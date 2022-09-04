#include <uapi/video/screen.h>
#include <uapi/service.h>
#include <proc/drvmaster.h>
#include <drivers/video/lfb.h>

#define MAXARGS_CLEARSCREEN 1

static UAPI_REQ_T current_request;


typedef enum {
    L_COLOR,
} INDEX_LABELS;

/*
 *  @param saved_req is the first request.
 *  @param req is the last request.
 *
 */

static UAPI_STATUS_T process_req(UAPI_REQ_T saved_req, UAPI_REQ_T req) {
    // Static variables for different requests.
    static UAPI_DATA_T color;
    static uint8_t data_index = L_COLOR;            // FIXME: CHANGE THIS WHEN MORE REQUESTS ARE ADDED.

    switch (saved_req) {
        case SCREENUAPI_CLEARSCREEN:
            if (data_index == L_COLOR) {
                color = req;                        // Treat req as other input data.
            }

            ++data_index;
            if (data_index >= MAXARGS_CLEARSCREEN) {
                lfb_clear_screen((uint32_t)color);
                return UAPI_OK;
            }
    }

    return UAPI_UNKNOWN_ERROR;
}


/*
 *  Cmdbursting is when the client (other end of the interface)
 *  sends bursts of requests that are treated as regular data.
 */


static UAPI_STATUS_T screen_uapi(UAPI_REQ_T req, UAPI_OUT_T* output) {
    // Ensure the process can actually use this uapi.
    if (verify_drvmaster(DRIVERTYPE_DISPLAY) < 0) {
        return UAPI_PERMISSION_DENIED;
    }

    static UAPI_REQ_T saved_req;

    switch (req) {
        case SCREENUAPI_CLEARSCREEN:
            saved_req = SCREENUAPI_CLEARSCREEN;
            break;
        default:
            process_req(saved_req, req);
            break;
    }

    return UAPI_OK;
}


void screen_uapi_init(void) {
    UAPI_APPEND_SERVICE(UAPI_SCREEN_SERVICE, screen_uapi);
}
