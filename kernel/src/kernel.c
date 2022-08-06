#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <common/log.h>

static void done(void) {
    for (;;) {
        __asm__("hlt");
    }
}

void _start(void) {
    kprintf(KINFO "Hello!");
    done();
}
