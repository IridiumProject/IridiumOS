#include <mm/pmm.h>
#include <arch/mem.h>
#include <common/log.h>
#include <common/panic.h>
#include <common/asm.h>
#include <limine.h>
#include <stddef.h>
#include <sync/mutex.h>

#define BITMAP_FREE 0x1

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


static struct limine_memmap_entry* biggest_chunk = NULL;
static struct limine_memmap_entry* second_biggest_chunk = NULL;
static uint8_t* bitmap = NULL;
static uint64_t bitmap_length = 0;
static uint64_t bitmap_i = 2;

uint64_t pmm_alloc(void) {
    static MUTEX_T lock = MUTEX_UNLOCKED;
    mutex_acquire(&lock);
    while (bitmap[bitmap_i] != BITMAP_FREE) {
        if (bitmap_i >= bitmap_length - 1) {
            return 0;
        }

        ++bitmap_i;
    }

    bitmap[bitmap_i] = 0;
    mutex_release(&lock);
    return 0x1000*bitmap_i++;
}


void pmm_free(uint64_t base) {
    MUTEX_T lock = MUTEX_UNLOCKED;
    mutex_acquire(&lock);
    for (uint32_t i = 0; i < bitmap_length; ++i) {
        if (0x1000*i == base) {
            bitmap[i] = BITMAP_FREE;
            bitmap_i = i;
            break;
        }
    }

    mutex_release(&lock);
}


void pmm_init(void) {
    // Locate the biggest chunks of physical memory.
    for (uint64_t i = 0; i < mmap_req.response->entry_count; ++i) {
        struct limine_memmap_entry* cur = mmap_req.response->entries[i];

        if (cur->type == LIMINE_MEMMAP_USABLE && biggest_chunk == NULL) {
            biggest_chunk = cur;
        } else if (cur->type == LIMINE_MEMMAP_USABLE && cur->length > biggest_chunk->length) {
            second_biggest_chunk = biggest_chunk;
            biggest_chunk = cur;
        }
    }

    if (biggest_chunk == NULL || second_biggest_chunk == NULL) {
        kprintf(KERN_PANIC "Not enough memory!\n");
        CLI; HLT;
    }

    bitmap = (uint8_t*)(uint64_t)PHYS_TO_HIGHER_HALF_DATA(second_biggest_chunk->base);
    bitmap_length = second_biggest_chunk->length;
}
