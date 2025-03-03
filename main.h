#include <stdbool.h>

#define MSGLEVEL 10

/** Default CLI options. */
static bool filemode = false;

#define log(lvl, fp, ...)         \
    if (MSGLEVEL >= (lvl)) {      \
        fprintf(fp, __VA_ARGS__); \
        fflush(fp);               \
    }
#define log_info(lvl, ...) log(lvl, stdout, __VA_ARGS__) /** Logs informational stuff to stdout. */
#define log_error(...) log(0, stderr, __VA_ARGS__) /** Logs errors to stderr. */
