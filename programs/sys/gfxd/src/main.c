#include <sys/drvctl.h>
#include <sys/sysreq.h>
#include <sys/process.h>
#include <ipcmd.h>

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
    while (1) {
        PSIGNAL_T ipcmd = psignal_fetch();

        // For now, only respond to init.
        if (ipcmd != 0 && PSIGNAL_EXTRACT_SOURCE(ipcmd) == 1) {
            switch (PSIGNAL_EXTRACT_SIGPAYLOAD(ipcmd) & 0xFF) {
                case GFXDSIG_CLEAR_SCREEN:
                sysreq(UAPI_SCREEN_SERVICE, 0x0, NULL);
                sysreq(UAPI_SCREEN_SERVICE, PSIGNAL_EXTRACT_SIGPAYLOAD(ipcmd) >> 8, NULL);
            }
        }
    }

    while (1);
    return 0;
}
