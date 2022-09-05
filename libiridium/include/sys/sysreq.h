#ifndef LIBIRIDIUM__SYS__SYSREQ_H
#define LIBIRIDIUM__SYS__SYSREQ_H

#include <sys/_sysreq_services.h>
#include <sys/syscall.h>
#include <stdint.h>
#include <stddef.h>

// Returns status.
// @param output: Output of the sys_req.
_syscallab uint64_t sysreq(UAPI_SERVICE_NUM_T service_num, uint64_t request, uint64_t* output);


#endif
