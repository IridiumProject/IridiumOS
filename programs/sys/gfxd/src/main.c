#include <sys/drvctl.h>
#include <sys/sysreq.h>

int a;

int main(void) {
    /*
     *  We, the display daemon have the PPERM_DRVCLAIM permission
     *  thus we can claim the display driver.
     *
     *  Attempt to claim the display driver and clear screen.
     *  TODO: Handle errors.
     *
     */
    ERRNO_T errno;
    if ((errno = claimdrv(DRIVERTYPE_DISPLAY)) == EXIT_SUCCESS) {  
        sysreq(UAPI_SCREEN_SERVICE, 0x0, NULL);
        sysreq(UAPI_SCREEN_SERVICE, 0x090909, NULL);
    }

    while (1);
    return 0;
}
