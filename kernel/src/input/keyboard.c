#include <input/keyboard.h>
#include <drivers/ps2/keyboard.h>
#include <uapi/service.h>

static UAPI_STATUS_T keyboard_uapi(UAPI_REQ_T req, UAPI_OUT_T* output) {
    switch (req) {
        case KEYBUAPI_FETCH_KEYSTROKE:
            *output = ps2_fetch_keystroke();
            return UAPI_OK;
    }

    return UAPI_INVALID_ARG;
}


void keyboard_uapi_init(void) {
    UAPI_APPEND_SERVICE(UAPI_KEYB_SERVICE, keyboard_uapi);
}
