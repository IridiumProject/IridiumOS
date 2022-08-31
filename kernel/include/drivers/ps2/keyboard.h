#ifndef PS2_KEYBOARD_H
#define PS2_KEYBOARD_H

#include <uapi/api_routine.h>


uint32_t ps2_fetch_keystroke(void);
void __attribute__((interrupt)) irq1_isr(void*);


#endif
