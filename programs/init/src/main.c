#include <stdint.h>

int main(void) {
    const char* const DISPLAY_DAEMON = "initrd/gfxd.sys";
    __asm__ __volatile__(
            "                                            \
            mov $0x3, %%rax;                             \
            mov %0, %%rbx;                               \
            mov $1 << 1, %%rcx; /* PPERM_DRVCLAIM */     \
            int $0x80" :: "m" (DISPLAY_DAEMON));

    while (1); 
    return 0;
}
