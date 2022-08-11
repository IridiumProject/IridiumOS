#ifndef LAPIC_H
#define LAPIC_H

#include <common/debug.h>
#include <stdint.h>

void lapic_init(void);
void lapic_send_eoi(void);
__attribute__((noreturn)) void cpu_reset(void);

#endif
