#include <stdint.h>
#include <service.h>
#include <sys/console.h>

void main(void) {
    con_out_u32(CONSOLE_CLEAR, 0);
    con_out_u32(CONSOLE_PURPLE "<==== IridiumD (init.sys) v0.0.1 ====>\n\n", 0);
    con_out_u32(CONSOLE_BOLD_YELLOW "Starting shell.. (shell.sys)\n\n", 0);
    _service_create("initrd/shell.sys", "Shell", "A system shell", PPERM_CONSOLE);
    while (1); 
}
