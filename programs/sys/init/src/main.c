#include <stdint.h>
#include <service.h>
#include <sigdef.h>
#include <sys/console.h>
#include <sys/process.h>
#include <sys/sysreq.h>
#include <sys/initrd.h>
#include <unistd.h>

static uint8_t psignal_lock = MUTEX_UNLOCKED;
static PID_T shell_pid;

static void psignal_callback(void) {
    mutex_acquire(&psignal_lock);
    PSIGNAL_T psignal = psignal_fetch();
    
    if (PSIGNAL_EXTRACT_SIGPAYLOAD(psignal) == PSIGNAL_KEYSTROKE) {
        // Tell shell.sys about keystroke.
        psignal_send(shell_pid, PSIGNAL_EXTRACT_SIGPAYLOAD(psignal));
    }

    // Done.
    mutex_release(&psignal_lock);
    exit();
}


void main(void) {
    con_out_u32("<==== IridiumD (init.sys) v0.0.1 ====>\n\n", RGB2HEX(138, 43, 226));
    con_out_u32("Starting shell.. (shell.sys)\n\n", DEFAULT_COLOR);
    psignal_hook(psignal_callback);
    shell_pid = _service_create("initrd/shell.sys", "Shell", "A system shell", PPERM_CONSOLE, 1);

    while (1); 
}
