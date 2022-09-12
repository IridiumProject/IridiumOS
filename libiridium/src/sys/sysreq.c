#include <sys/sysreq.h>

static uint64_t last_sysreq_status = 0;


_syscallab uint64_t sysreq(UAPI_SERVICE_NUM_T service_num, uint64_t request) {
    SYSCALLAB_INIT_ARG(UAPI_SERVICE_NUM_T, service_num);
    SYSCALLAB_INIT_ARG(uint64_t, request); 
    
    __asm__ __volatile__(
            "                                   \
            mov $0x1, %%rax; /* sys_req */      \
            mov %2, %%rbx;   /* Service num */  \
            mov %1, %%rcx;   /* Request num */  \
            int $0x80;       /* Syscall */      \
            mov %%rax, %0;   /* Status */       \
            mov %%rbx, %%rax;   /* Output */    \
            retq"  : "=m" (last_sysreq_status)
                   : "m" (SYSCALLAB_ARG(request)),
                     "m" (SYSCALLAB_ARG(service_num)));
}
