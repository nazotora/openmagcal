#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h> //For memcpy()
#include <math.h>
#include <fcntl.h> // fcntl
#include <unistd.h> //For getopts
#include <getopt.h> //Adding this include fixes the getopt error.
#include <wiringPi.h>
#include <wiringPiSPI.h>

#include <time.h>
#include <signal.h>

#include "fieldOrder.h"
#include "psu.h"
#include "main.h"

//Globals:
uint8_t *spiBuffer; //SPI buffer
int refB[3]; //Reference field (in nanotesla)
fieldOrderQueue_t* queue; //Linked list of fields
fieldOrderNode_t* latestOrder;
timer_t* timer; //Field changeover timer

void terminate(int signal) {
    //Shut off the timer. It is important that this happens first, as the timer could otherwise trigger during the termination routine.
    timer_delete(*timer);
    free(timer);
    //Shut down the magnetometer connection:
    wiringPiSPIClose(0);
    free(spiBuffer);
    //Get rid of the field order queue:
    fieldOrderQueue_free(queue);
    if (latestOrder != NULL) free(latestOrder);
    //Finally, close the TCP connection and exit the program:
    if (closeConnection()) {
        fprintf(stderr,"Failed to close socket!\n");
        exit(3);
    } else {
        exit(0);
    }
}

void readRefB() {
    spiBuffer[0] = 0x80 | 0x24; //MSB = 1 means read, MSB = 0 means write.
    wiringPiSPIDataRW(0,spiBuffer,10);
    uint32_t rX = 0 | spiBuffer[3] | (spiBuffer[2] << 8) | (spiBuffer[1] << 16);
    uint32_t rY = 0 | spiBuffer[6] | (spiBuffer[5] << 8) | (spiBuffer[4] << 16);
    uint32_t rZ = 0 | spiBuffer[9] | (spiBuffer[8] << 8) | (spiBuffer[7] << 16);
    int32_t x, y, z;
    memcpy(&x, &rX, 4);
    memcpy(&y, &rY, 4);
    memcpy(&z, &rZ, 4);
    refB[0] = SENSITIVITY * ((x << 8) >> 8);
    refB[1] = SENSITIVITY * ((y << 8) >> 8);
    refB[2] = SENSITIVITY * ((z << 8) >> 8);
}

void printRefB() {
    float bX = (float)refB[0] / 1000;
    float bY = (float)refB[1] / 1000;
    float bZ = (float)refB[2] / 1000;
    float bT = sqrtf(bX*bX + bY*bY + bZ*bZ);
    printf("B = %.3fx%+.3fy%+.3fz uT (|B| = %.3f)\n",bX, bY, bZ, bT);
}

int addOrder(char* buf) {
    float x, y, z;
    int t;
    int status = sscanf(buf, "%f %f %f %d", &x, &y, &z, &t);
    if (!status) {
        return 1;
    }
    fieldOrderQueue_enqueue(queue, fieldOrderNode_create(x, y, z, t));
    return 0;
}

void readinputfile(char* filepath) {
    FILE* fp = fopen(filepath, "r");
    if (fp == NULL) {
        log_error("File %s not found.", filepath);
        exit(1);
    }

    ssize_t read;
    char* line = NULL;
    size_t len = 0;
    while((read = getline(&line, &len, fp)) != -1) {
        if (addOrder(line)) {
            printf("Malformed field inclusion; exiting.\n");
            exit(1);
        }
    }

    fclose(fp);
    if (line) free(line);
    return;
}

void updateOrder(int signal) {
    if (!filemode) {
        printf("Enter new command:\n\t");
        char inBuffer[255];
        fgets(inBuffer, 255, stdin);
        if (addOrder(inBuffer)) {
            printf("Malformed Input!\n");
        }
    }
    if (fieldOrderQueue_isEmpty(queue)) {
        // Wait for 5 seconds for a new order:
        if (latestOrder != NULL) free(latestOrder);
        latestOrder = NULL;

        if (filemode) { //If we are in filemode, just end the program once we run out of commands.
            printf("Instructions completed!\n");
            terminate(SIGTERM);
        }
    } else {
        //Reset the update timer:
        latestOrder = fieldOrderQueue_dequeue(queue);
        struct itimerspec newInterval = {ZERO_TIME, {(latestOrder->t), 0}};
        timer_settime(*timer, 0, &newInterval, NULL);
    }
}

void updateField() {
    if (latestOrder) {
        //Compute the necessary currents:
        double iX = (1000 * latestOrder->x - (double)refB[0]) * SKEW[0];
        double iY = (1000 * latestOrder->y - (double)refB[1]) * SKEW[1];
        double iZ = (1000 * latestOrder->z - (double)refB[2]) * SKEW[2];
        printf("[DEBUG] Sending <%1.6f, %1.6f, %1.6f> to the PSU\n", iX, iY, iZ); //Debug print
        //Send the currents to the PSU:
        setAxisCurrent(fabs(iX), fabs(iY), fabs(iZ));
        //Update the sign pins:
        digitalWrite(SGN_X, iX < 0);
        digitalWrite(SGN_Y, iY < 0);
        digitalWrite(SGN_Z, iZ < 0);
    } else {
        if(!fieldOrderQueue_isEmpty(queue)) {
            // signal is non-authoritative in this instance.
            // restarting updateOrder loop.
            updateOrder(SIGUSR1);
            return;
        }
        // If no order, then default to supplying no power.
        setAxisCurrent(0.0, 0.0, 0.0);
        digitalWrite(SGN_X, 0);
        digitalWrite(SGN_Y, 0);
        digitalWrite(SGN_Z, 0);
    }
}

int main(int argc, char** argv) {
    int c; // why int... :(
    while ((c = getopt(argc, argv, "f:")) != -1) {
        // the random undefined variables in here are extern vars
        switch(c) {
            case 'f':
                filemode = true;
                filepath = optarg;
                break;
            case ':':
                log_error("Option -%c requires an operand.\n", optopt);
                exit(1);
                break;
            case '?':
                log_error("Unrecognized option: '-%c'\n", optopt);
                exit(1);
                break;
        }
    }

    //Set up the termination handler:
    struct sigaction termAction;
    termAction.sa_handler = terminate;
    sigemptyset(&termAction.sa_mask);
    sigaction(SIGINT, &termAction, NULL); //The termination handler is currently tied to SIGINT because that is usually how we are stopping it.

    printf("Initializing Queue...\n");
    queue = fieldOrderQueue_init();
    if (filemode) {
        // file reading.
        // currently for ease of use this just throws the entire thing in memory.
        readinputfile(filepath);
    }

    //Magnetometer setup:
    printf("Setting up Magnetometer...\n");
    spiBuffer = (uint8_t*)calloc(sizeof(char), 10);
    wiringPiSPISetupMode(0,9600,3); //Channel = 0, Speed = 9600 Bd, Mode = 3. Do not alter unless you know what you're doing!
    //Initiate continuous measurement mode:
    spiBuffer[0] = 0x01;
    spiBuffer[1] = 0b01110001;
    wiringPiSPIDataRW(0, spiBuffer, 2);

    // set file status flag of the stdin file handle to make it non-blocking.
    // this is required so that the read command doesnt lock the main thread
    //int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    //fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    printf("Setting up IP connection...\n");
    //Initialize connection to the PSU:
    initConnection(address, port);
    //Test the connection to the PSU:
    testConnection();

    printf("Configuring pins...\n");
    //Set up GPIO pins for sign:
    wiringPiSetup();
    pinMode(SGN_X, OUTPUT);
    pinMode(SGN_Y, OUTPUT);
    pinMode(SGN_Z, OUTPUT);

    printf("Configuring Timer...\n");
    //Set up the signal-loop system:
    timer = calloc(1, sizeof(timer_t*));
    struct sigaction timerAction;
    timerAction.sa_handler = updateOrder;
    sigemptyset(&timerAction.sa_mask);
    sigaction(SIGUSR1, &timerAction, NULL);
    struct sigevent timerEvent;
    timerEvent.sigev_notify = SIGEV_SIGNAL;
    timerEvent.sigev_signo = SIGUSR1;
    timerEvent.sigev_value.sival_int = 0;
    timer_create(CLOCK_REALTIME, &timerEvent, timer);

    printf("[DEBUG] Testing PSU connection AFTER timer has been set up but not enabled\n");
    testConnection();

    printf("Initializing...\n");
    //Start the first loop:
    updateOrder(0);



    ////loop:
    while (1) {
        readRefB();
        //printRefB();
        updateField();

        /*
        if (!filemode) {
            ssize_t n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
            if (n > 0) {
                // stuff in stdin :)
                buffer[n] = '\0';
                printf("%s\n", buffer);
                
                // try to add to queue.
                if (addOrder(buffer)) {
                    // add to command queue.
                    printf("Malformed field inclusion! Ignoring...\n");
                }
    
                // clear out what we had in stdin (not a typo)
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
            }
        }
        */

        //Used to sleep for 1 ms so that the PSU updates are not sent faster than the connection can handle.
        nanosleep(&CYCLE_TIME, NULL);
    }
    return 0;
}
