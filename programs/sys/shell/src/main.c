#include <sys/console.h>
#include <sys/process.h>
#include <sys/key.h>
#include <sys/sysreq.h>
#include <unistd.h>

#define LIVE_ENVIRON_PROMPT "live@iridium~!> "
#define PROMPT_COLOR RGB2HEX(0, 150, 150)

static void psignal_callback(void) {
    psignal_fetch();
    
    uint32_t keystroke_packet = sysreq(UAPI_KEYB_SERVICE, 0);
    char keystroke[2] = {KEYSTROKE_EXTRACT_CHARACTER(keystroke_packet), 0};

    if (KEYSTROKE_EXTRACT_SCANCODE(keystroke_packet) == KEY_ENTER) {
        con_out_u32("\n", PROMPT_COLOR);
        con_out_u32(LIVE_ENVIRON_PROMPT, PROMPT_COLOR);
    } else {
        con_out_u32(keystroke, PROMPT_COLOR);
    }

    exit();
}


void main(void) {
    psignal_hook(psignal_callback);
    con_out_u32(LIVE_ENVIRON_PROMPT, PROMPT_COLOR);
    while (1);
}
