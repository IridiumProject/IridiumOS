#include <mm/pmm.h>
#include <arch/mem.h>
#include <common/log.h>
#include <common/panic.h>
#include <common/asm.h>
#include <limine.h>
#include <stddef.h>

/*
 *  Module purpose:
 *
 *  Physical memory manager for allocating
 *  fixed-size frames.
 *
 */


static struct limine_memmap_request mmap_req = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};


static struct limine_memmap_entry* biggest_entry = NULL;


void pmm_init(void) {
    // Locate the biggest chunk of physical memory.
    for (uint64_t i = 0; i < mmap_req.response->entry_count; ++i) {
        struct limine_memmap_entry* cur = mmap_req.response->entries[i];

        if (cur->type == LIMINE_MEMMAP_USABLE && biggest_entry == NULL) {
            biggest_entry = cur;
        } else if (cur->type == LIMINE_MEMMAP_USABLE && cur->length > biggest_entry->length) {
            biggest_entry = cur;
        }
    }

    if (biggest_entry == NULL) {
        kprintf(KERN_PANIC "Not enough memory!\n");
        CLI; HLT;
    }
}
