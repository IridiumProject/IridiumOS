#include <service.h>
#include <sys/process.h>
#include <sys/syscall.h>
#include <stdint.h>


static struct Service services[MAX_SERVICES];


_syscallab static PID_T _initrd_spawn(const char* path, PPERM_T pmask) {
    __asm__ __volatile__(
            "                                            \
            mov $0x3, %%rax;                             \
            mov %0, %%rbx;                               \
            mov %1, %%rcx; /* PPERM_<@param pmask> */    \
            int $0x80;                                   \
            retq" :: "m" (path), "m" (pmask));
    
}

void _service_create(const char* path, const char* name, const char* description, PPERM_T pmask) {
    static struct Service service;
    service.perm_mask = pmask;
    service.service_name = name;
    service.service_description = description; 
    service.pid = _initrd_spawn(path, pmask);
    services[service.pid] = service;
}
