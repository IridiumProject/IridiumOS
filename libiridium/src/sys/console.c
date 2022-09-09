#include <sys/console.h>

_syscallab ERRNO_T con_out_u32(const char* str, uint32_t arg) {
    SYSCALLAB_INIT_ARG(const char*, str);
    SYSCALLAB_INIT_ARG(uint32_t, arg);

    __asm__ __volatile__(
            "\
            mov $0x6, %%rax; \
            mov %0, %%rbx; \
            mov %1, %%rcx; \
            int $0x80;     \
            retq" :: "m" (SYSCALLAB_ARG(str)), "m" (SYSCALLAB_ARG(arg)));
}
