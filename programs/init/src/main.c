#include <stdint.h>

int main(void) {
    while (1) {
        __asm__ __volatile__(
                "\
                mov $0x1, %rax; \
                mov $0x0, %rbx; \
                mov $0x0, %rcx; \
                int $0x80;       \
                shr $24, %rbx; \
                cmpq $0, %rbx; \
                je .done;        \
                mov $0x0, %rax; \
                int $0x80;      \
                .done:");
    }
    return 0;
}
