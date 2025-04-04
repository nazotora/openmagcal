#include <unistd.h>
#include "psu.h"

int main() {
    initConnection();
    testConnection();
    setAxisCurrent(0.01,0.02,0.03);
    sleep(5);
    setAxisCurrent(0.00,0.00,0.00);
    return 0;
}