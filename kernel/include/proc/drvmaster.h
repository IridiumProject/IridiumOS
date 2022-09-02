#ifndef PROC_DRIVER_MASTER_H
#define PROC_DRIVER_MASTER_H

#include <stdint.h>
#include <common/errno.h>

#define DRVCLAIM_SUCCESS 0
#define DRVCLAIM_FAILURE 1
#define DRVMASTER_MAX_SLAVES 25             // Max amount of drivers a drvmaster can own.


/*
 *  A drvmaster (driver master) is a system program that
 *  claims ownership to a specific driver group(s) during boot time.
 *
 *  This allows the drvmaster to block access from other programs
 *  to it's driver thus allowing the drvmaster to creating it's own 
 *  interface.
 *
 *  Example:
 *
 *  Let's say we have a graphics driver, and a display daemon claims
 *  ownership to the graphics driver at boot time. If another process
 *  tries to use a sysreq to interact with the driver directly, that process
 *  will get a premission denied error. But if it instead uses, let's say 
 *  a library to talk to the display daemon and calls a function like display_open_window()
 *  it can open a window safely!
 *
 *
 */


// THESE DRIVER CATEGORIES NEED TO BE CLAIMED.
typedef enum {
    DRIVERTYPE_NONE,
    DRIVERTYPE_DISPLAY,
} DRIVER_TYPE_T;
#define DRIVERTYPE_COUNT 1              // Excluding DRIVERTYPE_NONE.


/*
 *  Claims a driver.
 *
 *  NOTE: This only works during boot time (i.e the drv claim timeslice or DCTS).
 *
 */
ERRNO_T drv_claim(DRIVER_TYPE_T driver_type);

/*
 *  Returns 0 if all checks have passed (ensuring the current process owns the driver type, etc).
 *  Returns < 0 if check has failed (errno).
 *
 */

ERRNO_T verify_drvmaster(DRIVER_TYPE_T driver_type);


#endif
