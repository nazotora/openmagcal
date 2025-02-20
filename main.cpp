#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

//Constants:
const int SENSITIVITY = 13; //Sensitivity (inverse gain) in nT/LSB.

typedef struct float3 {
    float x;
    float y;
    float z;
} float3_t;

float3_t getAmbientField() {

}

int main(int argc, char** argv) {
    //setup:
    //The RM3100 wants CPOL=CPHA, so mode 0 or 3. 
    wiringPiSPIxSetupMode(0,0,9600,3); //Device=0?, Number=0?, Speed=9600?, Mode=3. Do not alter unless you know what you're doing!

    //loop:
    return 0;
}
