#include <stdint.h>
#include <service.h>
#include <perm.h>
#include <sys/process.h>
#include <gfx/draw.h>

int main(void) {
    const char* const DISPLAY_DAEMON = "initrd/gfxd.sys"; 
    _service_create(DISPLAY_DAEMON, "gfxd", "Graphics daemon", PPERM_DRVCLAIM);

    // Clear screen to remove boot logs.
    libgfx_clearscreen(0x090909);

    while (1); 
    return 0;
}
