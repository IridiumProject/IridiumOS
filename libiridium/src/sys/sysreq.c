#include <sys/sysreq.h>

_syscallab uint64_t sysreq(UAPI_SERVICE_NUM_T service_num, uint64_t request, uint64_t* output) {
    static uint64_t unused;
    if (output == NULL) {
        output = &unused;
    }
    
    __asm__ __volatile__(
            "                                   \
            mov $0x1, %%rax; /* sys_req */      \
            mov %2, %%rbx;   /* Service num */  \
            mov %1, %%rcx;   /* Request num */  \
            int $0x80;       /* Syscall */      \
            mov %%rbx, %0;   /* Output */       \
            retq"             : "=r" (*output)
                              : "m" (request),
                                "m" (service_num));
}
