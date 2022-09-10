#include <sys/initrd.h>

_syscallab PID_T _initrd_spawn(const char* path, PPERM_T pmask, uint8_t uses_console) {
    SYSCALLAB_INIT_ARG(const char*, path);
    SYSCALLAB_INIT_ARG(PPERM_T, pmask);
    SYSCALLAB_INIT_ARG(uint8_t, uses_console);
    __asm__ __volatile__(
            "                                            \
            mov $0x3, %%rax;                             \
            mov %0, %%rbx;                               \
            mov %1, %%rcx; /* PPERM_<@param pmask> */    \
            mov %2, %%rdx;                               \
            int $0x80;                                   \
            retq" :: "m" (SYSCALLAB_ARG(path)), "m" (SYSCALLAB_ARG(pmask)), "m" (SYSCALLAB_ARG(uses_console)));
    
}
