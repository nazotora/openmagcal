#include <wiringPi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"

uint8_t* spiBuffer;
int32_t refB[3];

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
    double bX = (float)refB[0] / 1000;
    double bY = (float)refB[1] / 1000;
    double bZ = (float)refB[2] / 1000;
    double bT = sqrtf(bX*bX + bY*bY + bZ*bZ);
    printf("Current: B = %.3fx%+.3fy%+.3fz uT (|B| = %.3f)\r",bX, bY, bZ, bT);
}

int main() {
    spiBuffer = (uint8_t*)calloc(sizeof(uint8_t), 10);
    wiringPiSPISetupMode(0,9600,3);
    spiBuffer[0] = 0x01;
    spiBuffer[1] = 0b01110001;
    wiringPiSPIDataRW(0, spiBuffer, 2);
    while(1) {
        readRefB();
        printRefB();
    }
}