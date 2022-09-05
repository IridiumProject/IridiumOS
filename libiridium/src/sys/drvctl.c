#include <sys/drvctl.h>
#include <errno.h>

static ERRNO_T errno;

_syscallab ERRNO_T claimdrv(DRIVER_TYPE_T driver_cat) {

    __asm__ __volatile__(
            "\
            mov $0x2, %%rax; \
            mov %0, %%rbx; \
            int $0x80; \
            retq" :: "m" (driver_cat));
}
