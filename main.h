#include <stdbool.h>

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


/** Constants */
const int SENSITIVITY = 13; //Sensitivity (inverse gain) in nT/LSB. This is for a 200 cycle measurement (the default).