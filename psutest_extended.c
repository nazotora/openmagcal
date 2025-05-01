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
uint8_t* spiBuffer; //SPI buffer, 10 bytes.
int refB[3]; //Reference field (in nanotesla)

char* ioBuffer; //Text input buffer, 256 bytes.
fieldOrderNode_t* latestOrder;
int countdown = 0;

void terminate(int signal) {
    if (latestOrder != NULL) free(latestOrder);
    free(spiBuffer);
    free(ioBuffer);
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
    printf("[DEBUG] B = %.3fx%+.3fy%+.3fz uT (|B| = %.3f)\n",bX, bY, bZ, bT);
}

void updateOrder(int signal) {
    if (latestOrder != NULL) {
        free(latestOrder);
        latestOrder = NULL;
    }

    double x = 0.0, y = 0.0, z = 0.0; //These are immediately initialized for safety reasons.
    int t = 0, invalid = FALSE;
    do {
        invalid = FALSE;
        printf("Enter new command:\n\t");
        fgets(ioBuffer, 255, stdin); //Place the input line into a buffer for safety.
        ////Note: We may want to flush out stdin after in case someone enters a line that is more than 255 characters long!

        //Read input and check for invalid values:
        if (sscanf(ioBuffer, "%lf %lf %lf %d", &x, &y, &z, &t) != 4) {
            printf("Error: Malformed input!\n");
            invalid = TRUE;
        } else if (fabs(x) > 160 || fabs(y) > 160 || fabs(z) > 160) {
            printf("Error: Magnetic field magnitude should not exceed 160 uT!\n");
            invalid = TRUE;
        } else if (t < 0 || t > 86400) { //Less than 0 seconds or More than 1 day.
            printf("Error: Invalid time parameter!\n");
            invalid = TRUE;
        }
    } while (invalid); //Make sure we actually have all the variables!

    latestOrder = fieldOrderNode_create(x, y, z, t); //Since latestOrder has been freed, it is safe to reuse the pointer.

    //Reset the update timer:
    countdown = latestOrder->t;
}

void updateField() {
    double iX = (1000 * latestOrder->x - (double)refB[0]) * SKEW[0];
    double iY = (1000 * latestOrder->y - (double)refB[1]) * SKEW[1];
    double iZ = (1000 * latestOrder->z - (double)refB[2]) * SKEW[2];
    #if DEBUG
    printf("[DEBUG] Currents set to <%1.6f, %1.6f, %1.6f> A\n", iX, iY, iZ); //Debug print
    #endif
    //Check to make sure that field does not exceed safe parameters, and stop it if it does:
    if (fabs(iX) > 0.6 || fabs(iY) > 0.6 || fabs(iZ) > 0.6) {
        fprintf(stderr, "WARNING: Maximum current exceeded safe parameters (600 mA). All currents set to 0.\n");
        iX = 0.0;
        iY = 0.0;
        iZ = 0.0;
    }
    //Send the currents to the PSU:
    setAxisCurrent(fabs(iX), fabs(iY), fabs(iZ));
    //Send the current sign to the pins:
    /* WARNING: CAUSES NETWORK FAILURE. DO NOT USE!
    digitalWrite(SGN_X, iX < 0);
    digitalWrite(SGN_Y, iY < 0);
    digitalWrite(SGN_Z, iZ < 0);
    */
}

int main(int argc, char** argv) {
    //Set up the termination handler:
    struct sigaction termAction;
    termAction.sa_handler = terminate;
    sigemptyset(&termAction.sa_mask);
    sigaction(SIGINT, &termAction, NULL); //The termination handler is currently tied to SIGINT because that is usually how we are stopping it.

    printf("Initializing Queue...\n");
    ioBuffer = (char*)calloc(sizeof(char), 256);
    latestOrder = (fieldOrderNode_t*)calloc(sizeof(fieldOrderQueue_t*), 1);

    printf("Setting up Magnetometer...\n");
    spiBuffer = (uint8_t*)calloc(sizeof(char), 10);
    wiringPiSPISetupMode(0,9600,3);
    spiBuffer[0] = 0x01;
    spiBuffer[1] = 0b01110001;
    wiringPiSPIDataRW(0, spiBuffer, 2);

    printf("Setting up IP connection...\n");
    //Initialize connection to the PSU:
    initConnection(address, port);
    //Test the connection to the PSU:
    testConnection();

    /* WARNING: CAUSES NETWORK FAILURE. DO NOT USE!
    printf("Configuring pins...\n");
    wiringPiSetup();
    pinMode(SGN_X, OUTPUT);
    pinMode(SGN_Y, OUTPUT);
    pinMode(SGN_Z, OUTPUT);
    */

    printf("Initializing...\n");
    //Start the first loop:
    updateOrder(0);

    ////loop:
    while (1) {
        readRefB(); //Updates refB[].
        #if DEBUG
        printRefB();
        #endif

        updateField(); //Updates what the PSU has set.

        nanosleep(&CYCLE_TIME, NULL); //Waits 1 second.

        if (--countdown < 0) updateOrder(0); //Updates the instruction when the timer is expired.
    }
    return 0;
}
