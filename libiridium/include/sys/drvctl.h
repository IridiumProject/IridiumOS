#ifndef LIBIRIDIUM__SYS__DRVCTL_H
#define LIBIRIDIUM__SYS__DRVCTL_H

#include <errno.h>

typedef enum {
    DRIVERTYPE_NONE,
    DRIVERTYPE_DISPLAY,
} DRIVER_TYPE_T;


__attribute__((naked)) ERRNO_T claimdrv(DRIVER_TYPE_T driver_cat);


#endif
