#include <stdint.h>




int main(void) {
    const char* const DISPLAY_DAEMON = "initrd/gfxd.sys";
    __asm__ __volatile__(
            "\
            mov $0x3, %%rax; \
            mov %0, %%rbx;  \
            mov $0x0, %%rcx; \
            int $0x80" :: "m" (DISPLAY_DAEMON));

    while (1) {
        __asm__ __volatile__(
                "\
                mov $0x1, %rax; \
                mov $0x0, %rbx; \
                mov $0x0, %rcx; \
                int $0x80;       \
                andq $1 << 24, %rbx; \
                jz .done;             \
                mov $0x0, %rax; \
                int $0x80;      \
                .done:");
    }
    
    while (1);
    return 0;
}
