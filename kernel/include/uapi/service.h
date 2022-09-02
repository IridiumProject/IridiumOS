#ifndef UAPI_SERVICE_H
#define UAPI_SERVICE_H

#include <stdint.h>
#include <uapi/service_def.h>
#include <uapi/sysreq.h>

/*
 *  When a new service is created, 
 *  place service name in the enum that is located in service_def.h
 *  and assign it's value to the previous
 *  services value + 1.
 *
 *  ** IMPORTANT **: You also need to increment SERVICE_COUNT.
 *  ** IMPORTANT **: Lastly, inside the serivce's uapi function put the following MACRO
 *  at the top:
 *  UAPI_APPEND_SERVICE(<service number you put in the enum>, <uapi routine ptr>).
 *
 *  Here's another example:
 *
 *  static UAPI_STATUS_T blah_uapi(UAPI_REQ_T req, UAPI_OUT_T* output) {
 *      UAPI_APPEND_SERVICE(UAPI_BLAH_SERVICE, blah_uapi);
 *      ..
 *  }
 *
 */


#define SERVICE_COUNT 2
#define UAPI_APPEND_SERVICE(service_num, uapi_routine_ptr)      \
    static uint8_t __uapi_append_lock = 0;                      \
    if (!(__uapi_append_lock)) {                                \
        __uapi_append_lock = 1;                                 \
        sysreq_append_service(service_num, uapi_routine_ptr);   \
    }

#endif
