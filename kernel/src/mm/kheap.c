#include <mm/kheap.h>
#include <mm/vmm.h>
#include <arch/mem.h>
#include <common/debug.h>
#include <common/log.h>
#include <stdint.h>

#define KHEAP_MAG 0xCA75C001
#define HEAP_SIZE	(4 * GiB)
#define HEAP_START_ADDR	0xFFFF900000000000
#define DATA_START(mem_block) (((char*) mem_block) + sizeof(struct KHeapBlock))

/*
 *  Each call to kmalloc() increases this value.
 *
 */
static size_t bytes_allocated = 0;
extern PML4* cur_pml4;                  // From mm/vmm.c

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

  // Map entire region.

  uint64_t tmp = (uint64_t)region;

  for (int i = 0; i < n_bytes; i += DEFAULT_PAGE_SIZE) {
    vmm_map_page(cur_pml4, tmp, PAGE_PRESENT | PAGE_WRITABLE);
    tmp += DEFAULT_PAGE_SIZE;
  }

  status = 0;       // Nothing went wrong, clear status.
  return region;
}


KHEAP_STATUS_T kheap_status(void) {
  return status;
}


void kfree(void* block) {
  struct KHeapBlock* region = block;

  if (region->mag != KHEAP_MAG) {
    status |= KHEAP_INVALID_FREE;
    return;
  }

  void* stop_addr = region - region->size;
  while (region != NULL && (uint64_t)region > (uint64_t)stop_addr) {
    ASSERT((uint64_t)(region) >= HEAP_START_ADDR);      // Ensure nothing funky happens.
    region->is_free = 1;

    uint64_t unmap_addr = (uint64_t)DATA_START(region);
    region = region->prev;
    vmm_unmap_page(cur_pml4, unmap_addr);
  }

  status = 0;       // Everything went well, status should be zero :)
}


void kheap_init(void) {
  vmm_map_page(cur_pml4, HEAP_START_ADDR, PAGE_PRESENT | PAGE_WRITABLE);
  heap_head = (struct KHeapBlock*)HEAP_START_ADDR;
  heap_head->size = 0;
  heap_head->next = NULL;
  heap_head->prev = NULL;
  heap_head->is_free = 0;
  heap_head->mag = KHEAP_MAG;
  heap_tail = heap_head;
}
