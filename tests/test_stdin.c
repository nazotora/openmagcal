#include <fcntl.h> // fcntl
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
    int counter = 0;
    char buffer[256];

    while(1) {
        // set file status flag of the stdin file handle to make it non-blocking.
        // this is required so that the read command doesnt lock the main thread
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

        counter++;
        ssize_t n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
        if (n > 0) {
            // stuff in stdin :)
            buffer[n] = '\0';
            printf("%s\n", buffer);
            
            // test the provided input is good.
            if (0) {
                // add to command queue.
            }

            // clear out what we had in stdin.
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }

        printf("%d\n", counter);
        sleep(1);
    }
}
