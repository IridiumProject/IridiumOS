#include <mm/vmm.h>
#include <mm/pmm.h>
#include <arch/mem.h>
#include <common/string.h>
#include <common/debug.h>

PML4* cur_pml4;


static inline uint64_t* read_cr3(void) {
    uint64_t cr3_value;
    asm volatile("mov %%cr3, %0" : "=r" (cr3_value));
    return (uint64_t*)(uint64_t)PHYS_TO_HIGHER_HALF_DATA(cr3_value);
}

static inline void invlpg(void* page_base)
{
    asm volatile("invlpg (%0)" :: "r" (page_base));

}

void vmm_map_page(PML4* pml4, uint64_t logical, uint32_t flags) {
    const uint64_t PML4_IDX = (logical >> 39) & 0x1FF;
    const uint64_t PDPT_IDX = (logical >> 30) & 0x1FF;
    const uint64_t PD_IDX = (logical >> 21) & 0x1FF;
    const uint64_t PT_IDX = (logical >> 12) & 0x1FF;

    if (!(pml4[PML4_IDX] & PAGE_PRESENT)) {
        uint64_t pdpt_alloc = pmm_alloc();
        memzero((void*)pdpt_alloc, DEFAULT_PAGE_SIZE);
        pml4[PML4_IDX] = (pdpt_alloc & PAGE_ADDR_MASK) | flags;
        vmm_map_page(pml4, pdpt_alloc, flags);
    }

    uint64_t* pdpt = (uint64_t*)(pml4[PML4_IDX] & PAGE_ADDR_MASK);

    if (!(pdpt[PDPT_IDX] & PAGE_PRESENT)) {
        uint64_t pdt_alloc = pmm_alloc();
        memzero((void*)pdt_alloc, DEFAULT_PAGE_SIZE);
        pdpt[PDPT_IDX] = (pdt_alloc & PAGE_ADDR_MASK) | flags;
        vmm_map_page(pml4, pdt_alloc, flags);
    }

    uint64_t* pdt = (uint64_t*)(pdpt[PDPT_IDX] & PAGE_ADDR_MASK);

    if (!(pdt[PD_IDX] & PAGE_PRESENT)) {
        uint64_t pt_alloc = pmm_alloc();
        memzero((void*)pt_alloc, DEFAULT_PAGE_SIZE);
        pdt[PD_IDX] = (pt_alloc & PAGE_ADDR_MASK) | flags;
        vmm_map_page(pml4, pt_alloc, flags);
    }

    uint64_t* page_tbl = (uint64_t*)(pdt[PD_IDX] & PAGE_ADDR_MASK);

    if (!(page_tbl[PT_IDX] & PAGE_PRESENT)) {
        uint64_t phys_map = pmm_alloc();
        page_tbl[PT_IDX] = (phys_map & PAGE_ADDR_MASK) | flags;
        invlpg((void*)phys_map);
    } else {
        page_tbl[PT_IDX] |= flags;
    }
}


void vmm_unmap_page(PML4* pml4, uint64_t logical) {
    logical = ALIGN_DOWN(logical, 0x1000);
    ASSERT(logical % 0x1000 == 0);
    const uint64_t PML4_IDX = (logical >> 39) & 0x1FF;
    const uint64_t PDPT_IDX = (logical >> 30) & 0x1FF;
    const uint64_t PD_IDX = (logical >> 21) & 0x1FF;
    const uint64_t PT_IDX = (logical >> 12) & 0x1FF;


    uint64_t* pdpt = (uint64_t*)(pml4[PML4_IDX] & PAGE_ADDR_MASK);
    uint64_t* pdt = (uint64_t*)(pdpt[PDPT_IDX] & PAGE_ADDR_MASK);
    uint64_t* page_tbl = (uint64_t*)(pdt[PD_IDX] & PAGE_ADDR_MASK);
    page_tbl[PT_IDX] = logical;
    invlpg((void*)logical);
}

void* vmm_alloc_page(void)
{
    uint64_t frame = pmm_alloc();
    if (frame == 0)
    {
        return NULL;
    }

    return (void*)((uint64_t)frame & PAGE_ADDR_MASK);
}

PML4* mkpml4(void) {
    uint64_t* new_pml = vmm_alloc_page();
    uint64_t* source_pml4 = read_cr3();

    for(uint16_t i = 0; i < 512; ++i) {
        new_pml[i] = source_pml4[i];
    }

    return (void*)((uint64_t)new_pml & PAGE_ADDR_MASK);
}


void vmm_init(void) {
    __asm__ __volatile__("mov %%cr3, %0" : "=r" (cur_pml4));
    cur_pml4 = mkpml4();
    __asm__ __volatile__("mov %0, %%cr3" :: "r" (cur_pml4));
}
