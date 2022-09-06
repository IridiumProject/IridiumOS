#include <gfx/draw.h>
#include <sys/process.h>
#include <gfx/def.h>


void libgfx_clearscreen(uint32_t color) {
    // CLEAR_SCREEN COMMAND = 0.
    psignal_send(GFXD_PID, (color << 8));
}
