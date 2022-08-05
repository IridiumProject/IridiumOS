#include <stdint.h>
#include <stddef.h>
#include <limine.h>


static void done(void) {
    for (;;) {
        __asm__("hlt");
    }
}

void _start(void) {
    done();
}
