#include <proc/tss.h>
#include <proc/proc.h>
#include <mm/vmm.h>
#include <mm/kheap.h>
#include <arch/mem.h>
#include <common/string.h>

#define KSTACK_SIZE 1000*3


static struct TSSEntry* tss;


void write_tss(void) {
    extern struct TSSDescriptor* gdt_tss;
        
    // Allocate and null out the TSS.
    tss = kmalloc(sizeof(struct TSSEntry));
    memzero(tss, sizeof(struct TSSEntry));

    uint64_t base = (uint64_t)tss;
    uint64_t stack = (uint64_t)(kmalloc(KSTACK_SIZE) + KSTACK_SIZE - 20);

    tss->rsp0Low = stack & 0xFFFFFFFF; 
    tss->rsp0High = stack >> 32;

    gdt_tss->seglimit = sizeof(struct TSSEntry);
    gdt_tss->g = 0;                                             // Limit is in bytes.

    gdt_tss->baseAddrLow = base & 0xFFFF;
    gdt_tss->baseAddrMiddle = (base >> 16) & 0xFF;
    gdt_tss->baseAddrOtherMiddle = (base >> 24) & 0xFF;
    gdt_tss->baseAddrUpper = base >> 32;
    gdt_tss->avl = 0;                                           // 0 for TSS.
    gdt_tss->dpl = 0;
    gdt_tss->p = 1;
    gdt_tss->reserved = 0;
    gdt_tss->type = 0x9;
    gdt_tss->zero = 0;
}
