#include <mm/vmm.h>
#include <mm/pmm.h>
#include <arch/mem.h>
#include <common/string.h>
#include <common/debug.h>
#include <sync/mutex.h>

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
    static MUTEX_T lock = MUTEX_UNLOCKED;
    mutex_acquire(&lock);

    const uint64_t pml4_idx = (logical >> 39) & 0x1ff;
    const uint64_t pdpt_idx = (logical >> 30) & 0x1ff;
    const uint64_t pd_idx = (logical >> 21) & 0x1ff;
    const uint64_t pt_idx = (logical >> 12) & 0x1ff;

    if (!(pml4[pml4_idx] & PAGE_PRESENT)) {
        uint64_t pdpt_alloc = pmm_alloc();
        memzero((void*)pdpt_alloc, DEFAULT_PAGE_SIZE);
        pml4[pml4_idx] = (pdpt_alloc & PAGE_ADDR_MASK) | flags;
        vmm_map_page(pml4, pdpt_alloc, flags);
    }

    uint64_t* pdpt = (uint64_t*)(pml4[pml4_idx] & PAGE_ADDR_MASK);

    if (!(pdpt[pdpt_idx] & PAGE_PRESENT)) {
        uint64_t pdt_alloc = pmm_alloc();
        memzero((void*)pdt_alloc, DEFAULT_PAGE_SIZE);
        pdpt[pdpt_idx] = (pdt_alloc & PAGE_ADDR_MASK) | flags;
        vmm_map_page(pml4, pdt_alloc, flags);
    }

    uint64_t* pdt = (uint64_t*)(pdpt[pdpt_idx] & PAGE_ADDR_MASK);

    if (!(pdt[pd_idx] & PAGE_PRESENT)) {
        uint64_t pt_alloc = pmm_alloc();
        memzero((void*)pt_alloc, DEFAULT_PAGE_SIZE);
        pdt[pd_idx] = (pt_alloc & PAGE_ADDR_MASK) | flags;
        vmm_map_page(pml4, pt_alloc, flags);
    }

    uint64_t* page_tbl = (uint64_t*)(pdt[pd_idx] & PAGE_ADDR_MASK);

    if (!(page_tbl[pt_idx] & PAGE_PRESENT)) {
        uint64_t phys_map = pmm_alloc();
        page_tbl[pt_idx] = (phys_map & PAGE_ADDR_MASK) | flags;
        invlpg((void*)logical);
    } else {
        page_tbl[pt_idx] &= ~(0b111);
        page_tbl[pt_idx] |= flags;
        invlpg((void*)logical);
    }

    mutex_release(&lock);
}


void vmm_unmap_page(PML4* pml4, uint64_t logical) {
    static MUTEX_T lock = MUTEX_UNLOCKED;
    mutex_acquire(&lock);

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
    mutex_release(&lock);
}

void* vmm_alloc_page(void)
{
    MUTEX_T lock = MUTEX_UNLOCKED;
    mutex_acquire(&lock);
    uint64_t frame = pmm_alloc();
    if (frame == 0)
    {
        return NULL;
    }

    vmm_map_page(vmm_get_vaddrsp(), frame, PAGE_PRESENT | PAGE_WRITABLE);
    mutex_release(&lock);
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


PML4* vmm_get_vaddrsp(void) {
    PML4 cr3;
    __asm__ __volatile__("mov %%cr3, %0" : "=r" (cr3));
    return (PML4*)cr3;
}


void vmm_init(void) {
    __asm__ __volatile__("mov %%cr3, %0" : "=r" (cur_pml4));
    cur_pml4 = mkpml4();
    __asm__ __volatile__("mov %0, %%cr3" :: "r" (cur_pml4));
}
