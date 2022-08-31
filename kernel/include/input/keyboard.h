#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <uapi/api_routine.h>

typedef enum {
    KEYBUAPI_FETCH_KEYSTROKE = 0
} KEYBOARD_UAPI_REQUEST;


void keyboard_uapi_init(void);

#endif
