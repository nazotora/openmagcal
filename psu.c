#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "psu.h"

const struct timespec MICROSEC = {0, 1000};

struct addrinfo* socketAddr;
int socketFD = -1;

char tcpBuffer[1024];

void initConnection(const char* address, const char* port) {
    // Set up the addr hints:
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    // Get address info
    if (getaddrinfo(address, port, &hints, &socketAddr) != 0) {
        fprintf(stderr, "Failed to set up address!\n");
        exit(1);
    }

    // Create the socket file descriptor:
    socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create IPv4 stream.
    if (socketFD == -1) {
        fprintf(stderr, "Failed to create socket!\n");
        exit(1);
    }

    //Enable nodelay for real-time sending of packets:
    int flag = 1;
    if (setsockopt(socketFD, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int)) == -1) {
        fprintf(stderr,"Failed to enable TCP_NODELAY!\n");
    }

    // Connect to the socket address:
    if (connect(socketFD, socketAddr->ai_addr, socketAddr->ai_addrlen) != 0) {
        fprintf(stderr, "Failed to connect to socket!\n");
        exit(2);
    }

    //Set voltage to 3, current to 0, and turn on the outputs:
    snprintf(tcpBuffer, 127, "SOUR:VOLT:SET CH1,3.0\nSOUR:VOLT:SET CH2,3.0\nSOUR:VOLT:SET CH3,3.0\n");
    send(socketFD, tcpBuffer, 90, 0);
    snprintf(tcpBuffer, 127, "SOUR:CURR:SET CH1,0.0\nSOUR:CURR:SET CH2,0.0\nSOUR:CURR:SET CH3,0.0\n");
    send(socketFD, tcpBuffer, 90, 0);
    snprintf(tcpBuffer, 127, "SOUR:OUTP:STAT CH1,OFF\nSOUR:OUTP:STAT CH2,OFF\nSOUR:OUTP:STAT CH3,OFF\n");
    send(socketFD, tcpBuffer, 90, 0);
}

void testConnection() {
    if (socketFD == -1) {
        fprintf(stderr, "Socket is not open!\n");
        return;
    }
    if (send(socketFD, "*IDN?", 5, 0) == -1) printf("[DEBUG] Failed to query PSU!\n");
    if (recv(socketFD, tcpBuffer, 1023, 0) == -1) {
        printf("[DEBUG] Failed to receive PSU IDN!\n");
    } else {
        printf("[DEBUG] PSU IDN: %1023s\n", tcpBuffer);
    }
}

void setAxisCurrent(double x, double y, double z) {
    if (socketFD == -1) {
        printf("[DEBUG] Cannot send, socket is not open!\n");
        fprintf(stderr, "Socket is not open!\n");
        return;
    }
    // 1 nanoamp is 0.000000001 amps, which should be able to be covered by %1.9f.
    // The nanosleep functions are to try and space the instructions out enough to allow proper configuration.
    snprintf(tcpBuffer, 127, "SOUR:CURR:SET CH1,%1.9f\nSOUR:CURR:SET CH2,%1.9f\nSOUR:CURR:SET CH3,%1.9f\n", x, y, z);
    int sent = send(socketFD, tcpBuffer, 90, 0);
    if (sent == -1) {
        printf("[DEBUG] Failed to send packet!\n");
        fprintf(stderr,"Failed to send packet!\n");
    } else {
        #if DEBUG
        printf("[DEBUG] Sent %d bytes to the PSU\n", sent);
        #endif
    }
}

int closeConnection() {
    snprintf(tcpBuffer, 127, "SOUR:OUTP:STAT CH1,OFF\nSOUR:OUTP:STAT CH2,OFF\nSOUR:OUTP:STAT CH3,OFF\n");
    send(socketFD, tcpBuffer, 90, 0);
    return close(socketFD);
}