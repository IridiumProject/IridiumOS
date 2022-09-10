#include <sys/process.h>
#include <sys/sysreq.h>


PID_T get_top_pid(void) {
    uint64_t out;
    sysreq(UAPI_PROCINFO_SERVICE, 0, &out);
    return (PID_T)out;
}


_syscallab PSIGNAL_T psignal_fetch(void) {
    __asm__ __volatile__(
            "                                           \
            mov $0x5, %rax; /* sys_read_psignal */      \
            int $0x80;                                  \
            retq");
}


_syscallab ERRNO_T psignal_send(PID_T dest, uint32_t payload) {
    SYSCALLAB_INIT_ARG(PID_T, dest);
    SYSCALLAB_INIT_ARG(uint32_t, payload);
    __asm__ __volatile__(
            "\
            mov $0x4, %%rax; \
            mov %0, %%rbx;   \
            mov %1, %%rcx;   \
            int $0x80;       \
            retq" :: "m" (SYSCALLAB_ARG(dest)), "m" (SYSCALLAB_ARG(payload)));
}


_syscallab void exit(void) {
    __asm__ __volatile__(
            "\
            mov $0x7, %rax; \
            int $0x80");

    while (1);
}
