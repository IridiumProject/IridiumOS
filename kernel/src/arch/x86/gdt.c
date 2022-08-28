#include <arch/x86/gdt.h>
#include <proc/tss.h>

#define GDT_TSS 0x9

static struct GDTDesc gdt[] = {
    {0}, 

    {
        .limit       = 0xFFFF,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b10011010,
        .granularity = 0b00000000,
        .base_hi     = 0x00
    },

    {
        .limit       = 0xFFFF,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b10010010,
        .granularity = 0b00000000,
        .base_hi     = 0x00
    },

    {
        .limit       = 0xFFFF,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b10011010,
        .granularity = 0b11001111,
        .base_hi     = 0x00
    },

    {
        .limit       = 0xFFFF,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b10010010,
        .granularity = 0b11001111,
        .base_hi     = 0x00
    },

    {
        .limit       = 0x0000,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b10011010,
        .granularity = 0b00100000,
        .base_hi     = 0x00
    }, 

    {
        .limit       = 0x0000,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b10010010,
        .granularity = 0b00000000,
        .base_hi     = 0x00
    },

    // User code: 0x7
    {
        .limit       = 0x0000,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b11111010,
        .granularity = 0b10101111,
        .base_hi     = 0x00
    },

    // User data: 0x8
    {
        .limit       = 0x0000,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b11110010,
        .granularity = 0b00000000,
        .base_hi     = 0x00
    },

    // 0x9.
    {0}, {0}, {0}
};


static struct GDTR gdtr = {
    sizeof(gdt) - 1,
    (uintptr_t)gdt
};


struct TSSDescriptor* gdt_tss = (struct TSSDescriptor*)&gdt[GDT_TSS];


void load_gdt(void) {
    __asm__ __volatile__("lgdt %0" :: "m" (gdtr));
}
