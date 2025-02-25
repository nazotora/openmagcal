#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

//Constants:
const int SENSITIVITY = 13; //Sensitivity (inverse gain) in nT/LSB.

//Globals:
uint8_t *buffer = (uint8_t*)malloc(4);

typedef struct float3 {
    float x;
    float y;
    float z;
} float3_t;

int getField(char axis) {
    if (axis == 'x') buffer[0] = 0x80 | 0x24;
    else if (axis == 'y') buffer[0] = 0x80 | 0x27;
    else if (axis == 'z') buffer[0] = 0x80 | 0x2A;
    wiringPiSPIDataRW(0, buffer, 4); //Should place the results in the 3 bytes that follow (HOPEFULLY).
    return buffer[1] + (buffer[2] << 8) + (buffer[3] << 16); //???
}

int main(int argc, char** argv) {
    //setup:
    //The RM3100 wants CPOL=CPHA, so mode 0 or 3. 
    wiringPiSPISetupMode(0,9600,3); //Channel=0?, Speed=9600?, Mode=3. Do not alter unless you know what you're doing!


    //loop:
    return 0;
}
