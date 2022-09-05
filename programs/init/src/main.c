#include <stdint.h>
#include <service.h>
#include <perm.h>

int main(void) {
    const char* const DISPLAY_DAEMON = "initrd/gfxd.sys";
    _service_create(DISPLAY_DAEMON, "gfxd", "Graphics daemon", PPERM_DRVCLAIM);

    while (1); 
    return 0;
}
