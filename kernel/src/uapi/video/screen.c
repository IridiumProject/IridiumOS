#include <uapi/video/screen.h>
#include <uapi/service.h>
#include <proc/drvmaster.h>
#include <drivers/video/lfb.h>

#define MAXARGS_CLEARSCREEN 1


typedef UAPI_REQ_T UAPI_DATA_T;
static UAPI_REQ_T current_request;


typedef enum {
    L_COLOR,
} INDEX_LABELS;


/*
 *  Cmdbursting is when the client (other end of the interface)
 *  sends bursts of requests that are treated as regular data.
 */


static UAPI_STATUS_T screen_uapi(UAPI_REQ_T req, UAPI_OUT_T* output) {
    // Ensure the process can actually use this uapi.
    if (verify_drvmaster(DRIVERTYPE_DISPLAY) < 0) {
        return UAPI_PERMISSION_DENIED;
    }

    static uint8_t cmdbursting = 0;
    static uint8_t data_index = L_COLOR;
    static UAPI_DATA_T color;

    switch (req) {
        case SCREENUAPI_CLEARSCREEN:
            cmdbursting = 1;
            break;
        default:
            if (data_index == L_COLOR) {
                color = req;                        // Treat req as other input data.
            }

            ++data_index;
            if (data_index >= MAXARGS_CLEARSCREEN) {
                cmdbursting = 0;
                lfb_clear_screen((uint32_t)color);
                return UAPI_OK;
            }
            break;
    }

    return UAPI_OK;
}


void screen_uapi_init(void) {
    UAPI_APPEND_SERVICE(UAPI_SCREEN_SERVICE, screen_uapi);
}
