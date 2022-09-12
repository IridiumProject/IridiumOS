#include <stdint.h>
#include <service.h>
#include <sigdef.h>
#include <sys/console.h>
#include <sys/process.h>
#include <sys/sysreq.h>
#include <sys/key.h>
#include <unistd.h>

static uint8_t psignal_lock = MUTEX_UNLOCKED;

static void psignal_callback(void) {
    mutex_acquire(&psignal_lock);
    PSIGNAL_T psignal = psignal_fetch();
    char key_char[2] = {KEYSTROKE_EXTRACT_CHARACTER(sysreq(UAPI_KEYB_SERVICE, 0)), 0};
    con_out_u32(key_char, DEFAULT_COLOR);
    mutex_release(&psignal_lock);
    exit();
}


void main(void) {
    con_out_u32("<==== IridiumD (init.sys) v0.0.1 ====>\n\n", RGB2HEX(138, 43, 226));
    con_out_u32("Starting shell.. (shell.sys)\n\n", DEFAULT_COLOR);
    // _service_create("initrd/shell.sys", "Shell", "A system shell", PPERM_CONSOLE);
    psignal_hook(psignal_callback);
    while (1); 
}
