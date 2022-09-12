#include <mm/kheap.h>
#include <mm/vmm.h>
#include <arch/mem.h>
#include <common/debug.h>
#include <common/log.h>
#include <common/asm.h>
#include <common/string.h>
#include <stdint.h>

#define KHEAP_MAG 0xCA75C001
#define HEAP_SIZE	(4 * GiB)
#define HEAP_START_ADDR (HIGHER_HALF_DATA)
#define DATA_START(mem_block) (((char*) mem_block) + sizeof(struct KHeapBlock))

/*
 *  Each call to kmalloc() increases this value.
 *
 */
static size_t bytes_allocated = 0;
extern PML4* vmm_get_vaddrsp();                  // From mm/vmm.c

struct KHeapBlock {
    uint8_t is_free : 1;
    uint64_t mag;
    size_t size;
    struct KHeapBlock* next;
    struct KHeapBlock* prev;
    char data_start[1];
};


static struct KHeapBlock* heap_head = NULL;
static struct KHeapBlock* heap_tail = NULL;
static KHEAP_STATUS_T status = 0;


static struct KHeapBlock* first_fit(size_t sz) {
    ASSERT(heap_head != NULL);

    struct KHeapBlock* block = heap_tail;

    /*
    *  Keep getting the next block until we find
    *  one that is free and has it's size >= @param sz.
    *
    */

    while (block != NULL) { 
        if (block->is_free && block->size >= sz) {
        return block;
        }

        if (block->next != NULL) {
        block->next->prev = block;      // Set the block's previous.
        }

        block = block->next;
    }

    /*
    *  Return NULL if no block that matches our needs
    *  was found.
    */

    return NULL;
}


void* kmalloc(size_t n_bytes) {
    // Ensure we have enough memory.
    if (bytes_allocated + n_bytes > HEAP_SIZE) {
        status |= KHEAP_NO_MEMORY;
        return NULL;
    }

    struct KHeapBlock* region = first_fit(n_bytes);
    struct KHeapBlock* tmp = (struct KHeapBlock*)DATA_START(region);

    for (int i = 0; i < n_bytes; i += DEFAULT_PAGE_SIZE) {
        vmm_map_page(vmm_get_vaddrsp(), (uint64_t)ALIGN_DOWN((uint64_t)tmp, 0x1000), PAGE_PRESENT | PAGE_WRITABLE);

        tmp += 0x1000;
    }

    // It is needed to make more space.
    if (region == NULL) {
        // Set next region in the linked list.
        char* next = DATA_START(heap_head + heap_head->size);
        heap_head->next = (struct KHeapBlock*)next;

        // Setup the region.
        region = (struct KHeapBlock*)next;
        region->is_free = 0;                  // Not free since we just allocated.
        region->size = n_bytes;               // Set the size.
        region->next = NULL;                  // No next, thus NULL.
        region->prev = heap_head;             // Set region's previous.
        region->mag = KHEAP_MAG;              // Set regions magic.
        heap_head = region;                   // Reassign the head of the heap to be the new region.
    }

    status = 0;       // Nothing went wrong, clear status.
    return DATA_START(region);
}


void* krealloc(void* old, size_t n_bytes) {
    char* new = kmalloc(n_bytes);
    memcpy(new, old, n_bytes);
    kfree(old);
    return new;
}


KHEAP_STATUS_T kheap_status(void) {
    return status;
}


void* kmalloc_user(size_t n_bytes) {
    void* block = kmalloc(n_bytes);

    for (uint64_t i = (uint64_t)ALIGN_DOWN((uint64_t)block, 0x1000); i < (uint64_t)(block + n_bytes); i += 0x1000) {
        vmm_map_page(vmm_get_vaddrsp(), i, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
    }

    return block;
}


void kfree(void* block) {
    /*
     *  - sizeof(struct KHeapBlock) because
     *  we want to get the blocks metadata.
     *
     */
    struct KHeapBlock* region = block - sizeof(struct KHeapBlock);

    if (region->mag != KHEAP_MAG) {
        status |= KHEAP_INVALID_FREE;
        return;
    }
    
    uint64_t stop_addr = (uint64_t)(block - region->size);
    struct KHeapBlock* cur_block = region;

    while (cur_block != NULL && block != DATA_START(cur_block)) {
        cur_block->is_free = 1;
        cur_block = cur_block->next;
    }

    status = 0;       // Everything went well, status should be zero :) 
}


void kheap_init(void) {
    vmm_map_page(vmm_get_vaddrsp(), HEAP_START_ADDR, PAGE_PRESENT | PAGE_WRITABLE);
    heap_head = (struct KHeapBlock*)HEAP_START_ADDR;
    heap_head->size = 0;
    heap_head->next = NULL;
    heap_head->prev = NULL;
    heap_head->is_free = 0;
    heap_head->mag = KHEAP_MAG;
    heap_tail = heap_head;
}
