#include <uapi/video/screen.h>
#include <uapi/service.h>
#include <proc/drvmaster.h>
#include <drivers/video/lfb.h>
#include <common/log.h>

#define MAXARGS_CLEARSCREEN 1
#define CLEARSCREEN_L_COLOR 0

#define MAXARGS_PUTPIXEL 3
#define PUTPIXEL_L_X 0                  // x pos.
#define PUTPIXEL_L_Y 1                  // y pos.
#define PUTPIXEL_L_COLOR 2              // Pixel color.

static UAPI_REQ_T current_request;


/*
 *  @param saved_req is the first request.
 *  @param req is the last request.
 *
 */

static UAPI_STATUS_T process_req(UAPI_REQ_T saved_req, UAPI_REQ_T req) {
    // Static variables for different requests.
    static UAPI_DATA_T color;
    static UAPI_DATA_T x;
    static UAPI_DATA_T y;
    static uint8_t data_index = 0;

    switch (saved_req) {
        case SCREENUAPI_CLEARSCREEN:
            if (data_index == CLEARSCREEN_L_COLOR) {
                color = req;                        // Treat req as other input data.
            }

            ++data_index;
            if (data_index >= MAXARGS_CLEARSCREEN) {
                lfb_clear_screen((uint32_t)color);
                data_index = 0;
                return UAPI_OK;
            }

            break;
        case SCREENUAPI_PUTPIXEL:
            if (data_index == PUTPIXEL_L_X) {
                x = req;
            } else if (data_index == PUTPIXEL_L_Y) {
                y = req;
            } else if (data_index == PUTPIXEL_L_COLOR) {
                color = req;
            }

            ++data_index;
            if (data_index >= MAXARGS_PUTPIXEL) {
                data_index = 0;
                lfb_put_pixel(x, y, color);
                return UAPI_OK;
            }
    }

    return UAPI_CMDBURST_CONTINUE;
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

    static uint8_t cmd_bursting = 0;
    static UAPI_REQ_T saved_req = 0;
     
    if (!(cmd_bursting)) {
        switch (req) {
            case SCREENUAPI_CLEARSCREEN:
                saved_req = SCREENUAPI_CLEARSCREEN;
                cmd_bursting = 1;
                break;
            case SCREENUAPI_PUTPIXEL:
                saved_req = SCREENUAPI_PUTPIXEL;
                cmd_bursting = 1;
                break;       
        }
    } else {
        UAPI_STATUS_T ret = process_req(saved_req, req);
            
        if (ret != UAPI_CMDBURST_CONTINUE) {
            saved_req = 0;
            cmd_bursting = 0;
        }    

        return ret;
    }

    return UAPI_OK;
}


void screen_uapi_init(void) {
    UAPI_APPEND_SERVICE(UAPI_SCREEN_SERVICE, screen_uapi);
}
