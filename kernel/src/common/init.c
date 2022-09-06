#include <common/init.h>
#include <common/elf.h>
#include <common/panic.h>
#include <common/log.h>
#include <common/asm.h>
#include <proc/ring3.h>
#include <fs/initrd.h>
#include <stdint.h>
#include <stddef.h>
#include <mm/vmm.h>


void start_init_system(void) {
    size_t unused;
    char* start_code = elf_get_entry("initrd/init.sys", &unused, (uint64_t)vmm_get_vaddrsp());

    if (start_code == NULL) {
        kprintf(KERN_PANIC "init.sys not found!\n");
        CLI; HLT;
    }

    void(*entry)(void) = (void*)start_code;
    switch_to_ring3(entry);
}
