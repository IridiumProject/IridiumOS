#include <sys/console.h>
#include <sys/process.h>

#define LIVE_ENVIRON_PROMPT "live@iridium~!> "


void main(void) {
    con_out_u32(CONSOLE_CYAN "Shell coming soon!", 0); 
    exit();
}
