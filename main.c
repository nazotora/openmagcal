#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h> // for getopts
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "main.h"

//Constants:
const int SENSITIVITY = 13; //Sensitivity (inverse gain) in nT/LSB.

//Globals:
uint8_t *buffer;
float reference[3];

typedef struct fieldOrder {
    float x;
    float y;
    float z;
    int t;
} fieldOrder_t;

void readRefMag() {
    buffer[0] = 0x80 | 0x24; //MSB = 1 means read, MSB = 0 means write.
    wiringPiSPIDataRW(0,buffer,10);
    uint32_t rX = 0 | buffer[3] | (buffer[2] << 8) | (buffer[1] << 16);
    uint32_t rY = 0 | buffer[6] | (buffer[5] << 8) | (buffer[4] << 16);
    uint32_t rZ = 0 | buffer[9] | (buffer[8] << 8) | (buffer[7] << 16);
    printf("Raw: X=%08X, Y=%08X, Z=%08X\n", rX, rY, rZ);
    int32_t x, y, z;
    memcpy(&x, &rX, 4);
    memcpy(&y, &rY, 4);
    memcpy(&z, &rZ, 4);
    x = ((x << 8) >> 8);
    y = ((y << 8) >> 8);
    z = ((z << 8) >> 8);
    printf("Formatted: X=%d, Y=%d, Z=%d\n", x, y, z);
}

void readinputfile(char* filepath) {
    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        log_error("File %s not found.", filepath);
        exit(1);
    }

    ssize_t read;
    char* line = NULL;
    size_t len = 0;
    while((read = getline(&line, &len, fp)) != -1) {
        // read in each line as its own input
        float x, y, z, t;
        sscanf(line, "%f %f %f %f", &x, &y, &z, &t);
    }

    fclose(fp);
    if (line) free(line);
    return;
}

int main(int argc, char** argv) {
    int c; // why int... :(
    while ((c = getopt(argc, argv, "f:")) != -1) {
        // the random undefined variables in here are extern vars
        switch(c) {
            case 'f':
                filemode = true;
                break;
            case ':':
                log_error("Option -%c requires an operand.\n", optopt);
                break;
            case '?':
                log_error("Unrecognized option: '-%c'\n", optopt);
                break;
        }
    }

    // file reading.
    // currently for ease of use this just throws the entire thing in memory.


    //setup:
    buffer = (uint8_t*)calloc(sizeof(char), 10);

    printf("Setting up Magnetometer");
    //The RM3100 wants CPOL=CPHA, so mode 0 or 3. 
    wiringPiSPISetupMode(0,9600,3); //Channel=0?, Speed=9600?, Mode=3. Do not alter unless you know what you're doing!
    //Initiate continuous measurement mode:
    buffer[0] = 0x01;
    buffer[1] = 0b01110001;
    wiringPiSPIDataRW(0, buffer, 2);

    //loop:
    while (1) {
        readRefMag();
        sleep(1); //Sleep for 1 second.
    }
    return 0;
}
