#ifndef PANIC_H
#define PANIC_H

#include <common/log.h>


#define KERN_PANIC "\033[H\033[2J\033[3J\e[0;31m**KERNEL PANIC**\n"
#define KPANIC_NOCLEAR "\e[0;31m**KERNEL PANIC**\n"
#define PPANIC_NOCLEAR "\n\n\e[0;31mProgram terminated: " WHITE        // Program panic.

// Called by exceptions.asm
void print_panic_msg(void);

#endif
