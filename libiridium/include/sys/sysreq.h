#ifndef LIBIRIDIUM__SYS__SYSREQ_H
#define LIBIRIDIUM__SYS__SYSREQ_H

#include <_sysreq_services.h>
#include <stdint.h>
#include <stddef.h>

// Returns status.
// @param output: Output of the sys_req.
__attribute__((naked))
uint64_t sysreq(UAPI_SERVICE_NUM_T service_num, uint64_t request, uint64_t* output);


#endif
