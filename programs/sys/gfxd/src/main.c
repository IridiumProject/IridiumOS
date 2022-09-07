#include <sys/drvctl.h>
#include <sys/sysreq.h>
#include <sys/process.h>
#include <ipcmd.h>
#include <def.h>

// Temporary.


#define DRAW_PIXEL(x, y, color) \
    sysreq(UAPI_SCREEN_SERVICE, 1, NULL);       \
    sysreq(UAPI_SCREEN_SERVICE, x, NULL);       \
    sysreq(UAPI_SCREEN_SERVICE, y, NULL);       \
    sysreq(UAPI_SCREEN_SERVICE, color, NULL);






int main(void) {
    /*
     *  We, the display daemon have the PPERM_DRVCLAIM permission
     *  thus we can claim the display driver.
     *
     *  Attempt to claim the display driver and clear screen.
     *  TODO: Handle errors.
     *
     */

    claimdrv(DRIVERTYPE_DISPLAY); 

    // Poll for signals.
    while (1) {
        PSIGNAL_T ipcmd = psignal_fetch();

        // For now, only respond to init.
        if (ipcmd != 0 && PSIGNAL_EXTRACT_SOURCE(ipcmd) == 1) {
            uint8_t cmd = PSIGNAL_EXTRACT_SIGPAYLOAD(ipcmd) & 0xFF;
            if (cmd == GFXDSIG_CLEAR_SCREEN) {
                sysreq(UAPI_SCREEN_SERVICE, 0x0, NULL);
                sysreq(UAPI_SCREEN_SERVICE, PSIGNAL_EXTRACT_SIGPAYLOAD(ipcmd) >> 8, NULL);
            }
        }
    }
    
    while (1);
    return 0;
}
