#include <unistd.h>
#include "psu.h"

int main() {
    initConnection("192.168.21.87", "5025");
    testConnection();
    setAxisCurrent(0.01,0.02,0.03);
    sleep(5);
    setAxisCurrent(0.00,0.00,0.00);
    sleep(1);
    closeConnection();
    return 0;
}