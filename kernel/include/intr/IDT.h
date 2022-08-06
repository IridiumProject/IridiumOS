#ifndef IDT_H
#define IDT_H

#include <stdint.h>


struct InterruptGateDescriptor {
    uint16_t isr_low16;
    uint16_t cs;
    uint8_t ist : 2;
    uint8_t zero : 1;
    uint8_t zero1 : 3;
    uint8_t attr : 4;
    uint8_t zero2 : 1;
    uint8_t dpl : 2;
    uint8_t p : 1;
    uint16_t isr_mid16;
    uint32_t isr_high32;
    uint32_t reserved;
};


struct __attribute__((packed)) IDTPtr {
    uint16_t limit;
    uint64_t base;
};


void idt_set_desc(uint8_t vector, void* isr, uint8_t flags);
void idt_install(void);

#endif
