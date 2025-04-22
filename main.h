#include <stdbool.h>
#include <time.h>
#include <signal.h>

#define INPUT_BUFFER_SIZE 256

#define log(fp, ...)          \
    fprintf(fp, __VA_ARGS__); \
    fflush(fp);
#define log_info(...)  log(stdout, __VA_ARGS__) /** Logs informational stuff to stdout. */
#define log_error(...) log(stderr, __VA_ARGS__) /** Logs errors to stderr. */

#ifdef DEBUG
    #define log_debug(...) log(stdout, __VA_ARGS__) /** Configurable debug logger. */
#else
    #define log_debug(...)
#endif

/** Default CLI options. */
static bool filemode = false;
static char* filepath;
static char* address = "192.168.21.87";
static char* port = "5025";


/** Constants */
const int SENSITIVITY = 13; //Sensitivity (inverse gain) in nT/LSB. This is for a 200 cycle measurement (the default).
const struct timespec ZERO_TIME = {0, 0}; //Zero time struct - used to specify no repeat interval in the main timer.
const double NANO = 1E-9; //Use this to convert a nano-prefixed value to its base form (A <- nA * NANO).
const double SKEW[] = 
{
    0.00000181818181818, //X-axis
    0.00000277700638711, //Y-axis
    0.00000296296296296  //Z-axis
}; //Skew factors - These do the transform from magnetic field values to current values.