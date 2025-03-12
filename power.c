#include <stdio.h>
#include <visa.h>

// Generated in part by ChatGPT o3-mini-high in accordance with 
// ECEDesign4890Syllabus2025SpringV1.pdf referencing EDU36311A-DC-Power-Supply-Programming-Guide

//This is a simple test script that performs an identity check with the Power Supply

int main() {
    ViSession rm, instr;
    ViStatus status;
    char response[256] = {0};
    ViUInt32 retCount = 0;

    // Open the VISA Resource Manager
    status = viOpenDefaultRM(&rm);
    if (status < VI_SUCCESS) {
        std::cout << "Error: Unable to open VISA Resource Manager" << std::endl;
        return 1;
    }

    // Open the instrument (replace with your actual resource string)
    status = viOpen(rm, "USB0::0x0957::0x0407::MY1234567::INSTR", VI_NULL, VI_NULL, &instr);
    if (status < VI_SUCCESS) {
        std::cout << "Error: Unable to open instrument" << std::endl;
        viClose(rm);
        return 1;
    }

    // Set timeout and termination character (newline)
    viSetAttribute(instr, VI_ATTR_TMO_VALUE, 5000);
    viSetAttribute(instr, VI_ATTR_TERMCHAR, '\n');
    viSetAttribute(instr, VI_ATTR_TERMCHAR_EN, VI_TRUE);

    // Send the *IDN? command to get the instrument identification
    viWrite(instr, (ViBuf)"*IDN?\n", 6, &retCount);

    // Read the response
    viRead(instr, (ViBuf)response, sizeof(response) - 1, &retCount);
    std::cout << "Instrument response: " << response << std::endl;

    // Close sessions
    viClose(instr);
    viClose(rm);
    
    return 0;
}
