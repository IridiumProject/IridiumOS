#ifndef LIBIRIDIUM__SYS__DRVCTL_H
#define LIBIRIDIUM__SYS__DRVCTL_H

#include <errno.h>
#include <sys/_syscall.h>

typedef enum {
    DRIVERTYPE_NONE,
    DRIVERTYPE_DISPLAY,
} DRIVER_TYPE_T;


_syscallab ERRNO_T claimdrv(DRIVER_TYPE_T driver_cat);


#endif
