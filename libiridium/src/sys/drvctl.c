#include <sys/drvctl.h>
#include <errno.h>

_syscallab ERRNO_T claimdrv(DRIVER_TYPE_T driver_cat) {
    SYSCALLAB_INIT_ARG(DRIVER_TYPE_T, driver_cat);
    __asm__ __volatile__(
            "\
            mov $0x2, %%rax; \
            mov %0, %%rbx; \
            int $0x80;  \
            retq" :: "m" (SYSCALLAB_ARG(driver_cat)));
}
