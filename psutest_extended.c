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
int refB[3]; //Reference field (in nanotesla)
fieldOrderNode_t* latestOrder;
int countdown = 0;

void terminate(int signal) {
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
    refB[0] = 2000;
    refB[1] = 4000;
    refB[2] = 6000;
}

void printRefB() {
    float bX = (float)refB[0] / 1000;
    float bY = (float)refB[1] / 1000;
    float bZ = (float)refB[2] / 1000;
    float bT = sqrtf(bX*bX + bY*bY + bZ*bZ);
    printf("B = %.3fx%+.3fy%+.3fz uT (|B| = %.3f)\n",bX, bY, bZ, bT);
}

void updateOrder(int signal) {
    //Reset the update timer:
    printf("Enter new command:\n\t");
    printf("100.0 -50.0 25.0 10\n");
    latestOrder->x = 100.0;
    latestOrder->y = -50.0;
    latestOrder->z = 25.0;
    latestOrder->t = 10;

    countdown = latestOrder->t;
}

void updateField() {
    double iX = (1000 * latestOrder->x - (double)refB[0]) * SKEW[0];
    double iY = (1000 * latestOrder->y - (double)refB[1]) * SKEW[1];
    double iZ = (1000 * latestOrder->z - (double)refB[2]) * SKEW[2];
    printf("[DEBUG] Sending <%1.6f, %1.6f, %1.6f> to the PSU\n", iX, iY, iZ); //Debug print
    //Send the currents to the PSU:
    setAxisCurrent(fabs(iX), fabs(iY), fabs(iZ));
}

int main(int argc, char** argv) {
    //Set up the termination handler:
    struct sigaction termAction;
    termAction.sa_handler = terminate;
    sigemptyset(&termAction.sa_mask);
    sigaction(SIGINT, &termAction, NULL); //The termination handler is currently tied to SIGINT because that is usually how we are stopping it.

    printf("Initializing \"Queue\"...\n");
    latestOrder = (fieldOrderNode_t*)calloc(sizeof(fieldOrderQueue_t*), 1);

    printf("Setting up \"Magnetometer\"...\n");

    printf("Setting up IP connection...\n");
    //Initialize connection to the PSU:
    initConnection(address, port);
    //Test the connection to the PSU:
    testConnection();

    printf("Configuring \"pins\"...\n");

    printf("Initializing...\n");
    //Start the first loop:
    updateOrder(0);

    printf("[DEBUG] Testing PSU connection after first order has been received\n");
    testConnection();

    ////loop:
    while (1) {
        readRefB();

        updateField();

        nanosleep(&CYCLE_TIME, NULL);

        if (--countdown < 0) updateOrder(0);
    }
    return 0;
}
