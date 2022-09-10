#ifndef LIBIRIDIUM__SYS__INITRD_H
#define LIBIRIDIUM__SYS__INITRD_H

#include <sys/_syscall.h>
#include <sys/process.h>


/*
 *  NOTE: This interface may be deprecated in the near future.
 */

_syscallab PID_T _initrd_spawn(const char* path, PPERM_T pmask, uint8_t uses_console);


#endif
