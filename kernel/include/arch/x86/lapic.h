#ifndef LAPIC_H
#define LAPIC_H

#include <common/debug.h>

void lapic_init(void);
void lapic_send_eoi(void);


#endif