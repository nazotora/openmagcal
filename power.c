
// Generated in part by ChatGPT o3-mini-high in accordance with 
// ECEDesign4890Syllabus2025SpringV1.pdf referencing Siglent SPD4000X_UserManual

#include <stdio.h>
#include <stdlib.h>
#include <visa.h>

int main(void) {
    ViSession defaultRM, instr;
    ViStatus status;
    char buffer[256];

    // Open a session to the VISA Resource Manager
    status = viOpenDefaultRM(&defaultRM);
    if (status < VI_SUCCESS) {
        printf("Could not open a session to the VISA Resource Manager.\n");
        return -1;
    }

    // Open a session to the instrument.
    // Replace the resource string below with your instrument's address.
    status = viOpen(defaultRM, "USB0::0xF4EC::SPD41A1D800196::INSTR", VI_NULL, VI_NULL, &instr);
    if (status < VI_SUCCESS) {
        printf("Could not connect to the instrument.\n");
        viClose(defaultRM);
        return -1;
    }

    // Clear the instrument buffers
    viClear(instr);

    // Send the *IDN? command to query the instrument's identification
    status = viPrintf(instr, "*IDN?\n");
    if (status < VI_SUCCESS) {
        printf("Error writing the command to the instrument.\n");
        viClose(instr);
        viClose(defaultRM);
        return -1;
    }

    // Read the response from the instrument
    status = viScanf(instr, "%t", buffer);
    if (status < VI_SUCCESS) {
        printf("Error reading the response from the instrument.\n");
        viClose(instr);
        viClose(defaultRM);
        return -1;
    }

    printf("Instrument Identification: %s\n", buffer);

    // Close the sessions
    viClose(instr);
    viClose(defaultRM);

    return 0;
}
