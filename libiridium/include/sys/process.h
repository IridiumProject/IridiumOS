#ifndef LIBIRIDIUM__SYS__PROCESS_H
#define LIBIRIDIUM__SYS__PROCESS_H

#include <stdint.h>
#include <sys/_syscall.h>
#include <sys/_psignaldef.h>
#include <errno.h>


typedef int16_t PID_T;
typedef uint32_t PPERM_T;
typedef uint64_t PSIGNAL_T;


/*
 *  Returns PID of most recently created
 *  process.
 */

PID_T get_top_pid(void);
_syscallab PSIGNAL_T psignal_fetch(void);
_syscallab ERRNO_T psignal_send(PID_T dest, uint32_t payload);
_syscallab void psignal_hook(void* function_ptr);
_syscallab void exit(void);

#endif
