#include <proc/tss.h>
#include <proc/proc.h>
#include <mm/vmm.h>
#include <mm/kheap.h>
#include <arch/mem.h>
#include <common/string.h>

static struct TSSEntry* tss;


void write_tss(void) {
    extern struct Process* current_task;
    extern struct TSSDescriptor* gdt_tss;

    tss = kmalloc(sizeof(struct TSSEntry)); 

    uint64_t stack = (uint64_t)vmm_alloc_page();
    stack += (DEFAULT_PAGE_SIZE - 1);

    memzero(&tss, sizeof(struct TSSEntry));
    tss->rsp0Low = stack & 0xFFFFFFFF;
    tss->rsp0High = (stack >> 32);

    uint64_t tssp = (uint64_t)tss;
    gdt_tss->seglimit = sizeof(struct TSSDescriptor);
    gdt_tss->baseAddrLow = tssp & 0xFFFF;
    gdt_tss->baseAddrMiddle = (tssp >> 16) & 0xFF;
    gdt_tss->baseAddrUpper = (tssp >> 24) & 0xFFFFFFFF;
    gdt_tss->type = 0x9;
    gdt_tss->avl = 0;
    gdt_tss->zero = 0;
    gdt_tss->dpl = 3;
    gdt_tss->p = 1;
    gdt_tss->g = 1;
}
