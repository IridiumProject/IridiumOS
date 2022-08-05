#ifndef PANIC_H
#define PANIC_H

#define KERN_PANIC "\033[H\033[2J\033[3J\e[0;31m**KERNEL PANIC**\n"

// Called by exceptions.asm
void print_panic_msg(void);

#endif
