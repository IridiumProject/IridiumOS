#ifndef KERRNO_H
#define KERRNO_H

enum {
    EXIT_SUCCESS,           // Everything went alright! :D
    EXIT_FAILURE,           // Some unlisted failure.
    ENOBUFS,                // No buffer space left.
    EPERM,                  // Permission denied.
    ENOMEM,                 // Out of memory.
    ENOENT,                 // File or directory does not exist.
    EDRVCLAIMED,            // Daemons can get this error if a double claim occurs.
};

typedef int16_t ERRNO_T;


#endif
