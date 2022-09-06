#ifndef MM_VMM_H
#define MM_VMM_H

#include <stdint.h>

typedef uint64_t PML4;


#define LOAD_CR3(cr3) \
    __asm__ __volatile__("mov %0, %%cr3" :: "r" (cr3));


typedef enum {
    PAGE_PRESENT = (1 << 0),
    PAGE_WRITABLE = (1 << 1),
    PAGE_USER = (1 << 2)
} PAGE_BITS;

void vmm_map_page(PML4* pml4, uint64_t logical, uint32_t flags);
void vmm_unmap_page(PML4* pml4, uint64_t logical);
void vmm_init(void);
void* vmm_alloc_page(void);
PML4* mkpml4(void);
PML4* vmm_get_vaddrsp(void);

#endif
