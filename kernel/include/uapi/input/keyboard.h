#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <uapi/api_routine.h>

enum {
    KEYBUAPI_FETCH_KEYSTROKE = 0
};


void keyboard_uapi_init(void);

#endif
