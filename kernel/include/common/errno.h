#ifndef ERRNO_H
#define ERRNO_H

#include <stdint.h>

enum {
    EXIT_SUCCESS,           // Everything went alright! :D
    EXIT_FAILURE,           // Some unlisted failure.
    ENOBUFS,                // No buffer space left.
    EPERM,                  // Permission denied.
    ENOMEM,                 // Out of memory.
    ENOENT,                 // File or directory does not exist.
    EREJECTED,              // Request rejected.
    EDRVCLAIMED,            // Daemons can get this error if a double claim occurs.
};

typedef int16_t ERRNO_T;


#endif
