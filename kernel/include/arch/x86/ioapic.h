#ifndef IOAPIC_H
#define IOAPIC_H

#include <stdint.h>


void ioapic_init(void);
void ioapic_set_entry(uint8_t index, uint64_t data);


#endif
