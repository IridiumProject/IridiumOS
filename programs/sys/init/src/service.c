#include <service.h>
#include <sys/initrd.h>
#include <stdint.h>


static struct Service services[MAX_SERVICES];


PID_T _service_create(const char* path, const char* name, const char* description, PPERM_T pmask, uint8_t uses_console) {
    static struct Service service;
    service.perm_mask = pmask;
    service.service_name = name;
    service.service_description = description; 
    PID_T ret = service.pid = _initrd_spawn(path, pmask, uses_console);
    services[service.pid] = service;
    return ret;
}
