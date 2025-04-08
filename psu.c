#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "psu.h"

const char* ADDRESS = "192.168.21.87";
const char* PORT = "5025";
const struct timespec MICROSEC = {0, 1000};

struct addrinfo* socketAddr;
int socketFD = -1;

char tcpBuffer[128];

void initConnection() {
    // Set up the addr hints:
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    // Get address info
    if (getaddrinfo(ADDRESS, PORT, &hints, &socketAddr) != 0) {
        fprintf(stderr, "Failed to set up address!\n");
        exit(1);
    }

    // Create the socket file descriptor:
    socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create IPv4 stream.
    if (socketFD == -1) {
        fprintf(stderr, "Failed to create socket!\n");
        exit(1);
    }

    // Connect to the socket address:
    if (connect(socketFD, socketAddr->ai_addr, socketAddr->ai_addrlen) != 0) {
        fprintf(stderr, "Failed to connect to socket!\n");
        exit(2);
    }
}

void testConnection() {
    if (socketFD == -1) {
        fprintf(stderr, "Socket is not open!\n");
        return;
    }
    send(socketFD, "*IDN?", 5, 0);
    recv(socketFD, tcpBuffer, 26, 0);
    printf("%26s", tcpBuffer);
}

void setAxisCurrent(float x, float y, float z) {
    if (socketFD == -1) {
        fprintf(stderr, "Socket is not open!\n");
        return;
    }
    // "SOUR:CURR:SET CH1,0.000000" Example of structure, 26 characters + possible newline or null character.
    // The nanosleep functions are to try and space the instructions out enough to allow proper configuration.
    snprintf(tcpBuffer, 127, "SOUR:CURR:SET CH1,%1.6f\nSOUR:CURR:SET CH2,%1.6f\nSOUR:CURR:SET CH3,%1.6f\n", x, y, z);
    send(socketFD, tcpBuffer, 81, 0);
}

void closeConnection() {
    shutdown(socketFD, SHUT_RDWR);
}
/*
void setAxisCurrent(float x, float y, float z) {
    if (socketFD == -1) {
        fprintf(stderr, "Socket is not open!\n");
        return;
    }
    // "SOUR:CURR:SET CH1,0.000000" Example of structure, 26 characters + possible newline or null character.
    // The nanosleep functions are to try and space the instructions out enough to allow proper configuration.
    snprintf(tcpBuffer, 31, "SOUR:CURR:SET CH1,%1.6f\n",x);
    send(socketFD, tcpBuffer, 27, 0);
    nanosleep(&MICROSEC, NULL);

    snprintf(tcpBuffer, 31, "SOUR:CURR:SET CH2,%1.6f\n",y);
    send(socketFD, tcpBuffer, 27, 0);
    nanosleep(&MICROSEC, NULL);

    snprintf(tcpBuffer, 31, "SOUR:CURR:SET CH3,%1.6f\n",z);
    send(socketFD, tcpBuffer, 27, 0);
    nanosleep(&MICROSEC, NULL);
}
*/