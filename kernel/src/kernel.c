#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <common/log.h>
#include <intr/intr.h>

static void done(void) {
    for (;;) {
        __asm__("hlt");
    }
}

void _start(void) {
    intr_init();
    kprintf(KINFO "Interrupts init: OK\n");
    done();
}
