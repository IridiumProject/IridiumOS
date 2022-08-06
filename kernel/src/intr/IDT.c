#include <intr/IDT.h>


static struct InterruptGateDescriptor idt[256];
static struct IDTPtr idt_ptr;


void idt_set_desc(uint8_t vector, void* isr, uint8_t flags) {
    uint64_t addr = (uint64_t)isr;
    struct InterruptGateDescriptor* desc = &idt[vector];
    desc->isr_low16 = addr & 0xFFFF;
    desc->isr_mid16 = (addr >> 16) & 0xFFFF;
    desc->isr_high32 = addr >> 32;
    desc->cs = 0x28;
    desc->dpl = 3;
    desc->p = 1;
    desc->ist = 0;
    desc->zero = 0;
    desc->zero1 = 0;
    desc->zero2 = 0;
    desc->attr = flags;
    desc->reserved = 0;
}


void idt_install(void) {
    idt_ptr.limit = sizeof(struct InterruptGateDescriptor) * 256 - 1;
    idt_ptr.base = (uint64_t)&idt;
    __asm__ __volatile__("lidt %0" :: "m" (idt_ptr));
}
