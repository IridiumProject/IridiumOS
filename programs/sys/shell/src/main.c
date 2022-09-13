#include <sys/console.h>
#include <sys/process.h>
#include <sys/key.h>
#include <sys/sysreq.h>
#include <unistd.h>
#include <string.h>

#define LIVE_ENVIRON_PROMPT "live@iridium~!> "
#define PROMPT_COLOR RGB2HEX(0, 150, 150)
#define COMMAND_BUF_SIZE 7


static char cmdbuf[COMMAND_BUF_SIZE];
static uint8_t cmdbuf_idx = 0;

static void psignal_callback(void) {
    psignal_fetch();
    
    uint32_t keystroke_packet = sysreq(UAPI_KEYB_SERVICE, 0);
    char keystroke[2] = {KEYSTROKE_EXTRACT_CHARACTER(keystroke_packet), 0};
    char fetch[] = {0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x0A, 0x5C, 0x20, 0x20, 0x7C, 0x7C, 0x20, 0x20, 0x2F, 0x0A, 0x20, 0x5C, 0x20, 0x7C, 0x7C, 0x20, 0x2F, 0x0A, 0x20, 0x20, 0x5C, 0x7C, 0x7C, 0x2F, 0x0A, 0x20, 0x20, 0x20, 0x5C, 0x2F, 0};

    if (KEYSTROKE_EXTRACT_SCANCODE(keystroke_packet) == KEY_ENTER) {
        con_out_u32("\n", PROMPT_COLOR);
        uint8_t cmd_success = 0;

        // Check for basic built in commands.
        if (strncmp(cmdbuf, "fetch", 2) == 0) {
            con_out_u32(fetch, PROMPT_COLOR);
            con_out_u32(" Iridium OS (early stages)\n\n", PROMPT_COLOR);
            cmd_success = 1;
        } else if (strncmp(cmdbuf, "h", 1) == 0) {
            con_out_u32("Bogos binted?\n", PROMPT_COLOR);
            cmd_success = 1;
        }

        // Write out command not found message.
        if (!(cmd_success)) {
            con_out_u32("Sorry, I didn't understand your command!\n", PROMPT_COLOR);
        }
        
        cmdbuf_idx = 0;
        con_out_u32(LIVE_ENVIRON_PROMPT, PROMPT_COLOR);
    } else if (cmdbuf_idx < COMMAND_BUF_SIZE) {
        cmdbuf[cmdbuf_idx++] = keystroke[0];
        con_out_u32(keystroke, PROMPT_COLOR);
    }

    exit();
}


void main(void) {
    psignal_hook(psignal_callback);
    con_out_u32(LIVE_ENVIRON_PROMPT, PROMPT_COLOR);
    cmdbuf_idx = 0; 

    while (1);
}
