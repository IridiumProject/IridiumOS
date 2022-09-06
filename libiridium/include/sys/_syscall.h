#ifndef LIBIRIDIUM__SYS__SYSCALL_H
#define LIBIRIDIUM__SYS__SYSCALL_H

#include <stdint.h>


// Syscall abstraction macro (put in front of syscall abstraction function).
// The '_' prefix in this case is ONLY to tell that this is an attribute.
// '_' prefix with macros and functions mean two different things.
#define _syscallab __attribute__((naked))
#endif
