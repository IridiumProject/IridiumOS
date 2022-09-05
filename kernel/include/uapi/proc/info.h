#ifndef UAPI_PROC_INFO_H
#define UAPI_PROC_INFO_H

/*
 *  User API for information about process related stuff.
 */


enum {
    // Fetches PID of most recently created process.
    PROCINFO_UAPI_GET_PIDTOP = 0,
};


void procinfo_uapi_init(void);

#endif
