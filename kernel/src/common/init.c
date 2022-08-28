#include <common/init.h>
#include <common/elf.h>
#include <proc/proc.h>
#include <mm/vmm.h>
#include <stdint.h>
#include <stddef.h>

#define INIT_START_VADDR 0x500


extern struct Process* current_task;


void start_init_system(void) {
    // Map a page at 0x500 for the init service.
    vmm_map_page((PML4*)current_task->context[PCTX_CR3], INIT_START_VADDR, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);

    // Move data from kmalloc'd memory to our newly mapped memory.
    uint64_t current_vaddr = INIT_START_VADDR;
    size_t code_size;
    char* start_code = elf_get_entry("initrd/init.sys", &code_size);
    char* init_start = (char*)INIT_START_VADDR;

    for (size_t i = 0; i < code_size; ++i) {
        init_start[i] = start_code[i];
        ++current_vaddr;

        if (current_vaddr % 0x1000) {
            vmm_map_page((PML4*)current_task->context[PCTX_CR3], current_vaddr, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
        }
    }

    void(*init)(void) = (void*)INIT_START_VADDR;
    init();
}
