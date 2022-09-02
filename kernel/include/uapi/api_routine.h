#ifndef UAPI_API_ROUTINE_H
#define UAPI_API_ROUTINE_H

#include <stdint.h>

typedef uint16_t UAPI_SERVICE_T;
typedef uint64_t UAPI_REQ_T;
typedef uint64_t UAPI_OUT_T;
typedef enum {
    UAPI_OK = 0,
    UAPI_INVALID_ARG = 1,
    UAPI_INVALID_SERVICE = 2,
    UAPI_PERMISSION_DENIED = 3,
    UAPI_CMDBURST_NOT_STARTED = 4,
    UAPI_UNKNOWN_ERROR = 5,
} UAPI_STATUS_T;

/*
 *  UAPI routine example:
 *
 *  static UAPI_STATUS_T driver_uapi(UAPI_REQ_T req, UAPI_OUT_T* output);
 *  
 *  Where @param output can be assigned an output value like so:
 *
 *  *output = 1337;
 *
 *  Set to zero if unused:
 *
 *  *output = 0;
 *
 *  A service is a specific driver, this is used when you tell sysreq
 *  what service you need to request from.
 *
 *  For example if you want to talk to the keyboard driver you will
 *  set the service to match what identifies the keyboard driver.
 *
 *  A request is the action you want from a driver.
 *  For example, you could request a keystroke from 
 *  the keyboard driver.
 *
 */

#endif
