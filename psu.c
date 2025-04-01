#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>

const char* ADDRESS = "192.168.21.87";
const uint16_t PORT = 5025;
struct sockaddr_in socketAddr;
int socketFD = -1;

void initConnection() {
    socketAddr.sin_addr.s_addr = inet_addr(ADDRESS);
    socketAddr.sin_port = PORT;
    socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create IPv4 stream.
    if (socketFD == -1) {
        fprintf(stderr, "Failed to create socket!\n");
        exit(1);
    }
    if (connect(socketFD, &socketAddr, sizeof(socketAddr)) != 0) {
        fprintf(stderr, "Failed to connect to socket!\n");
        exit(2);
    }
}

void setAxisCurrent(float x, float y, float z) {
    if (socketFD == -1) {
        fprintf(stderr, "Socket is not open!\n");
        return;
    }
    "SOUR:CURR:SET CH1,0.000000" //Example of structure, 26 characters + possible newline or null character.
    send(socketFD,);
}