#include <common/init.h>
#include <common/elf.h>
#include <proc/ring3.h>
#include <stdint.h>
#include <stddef.h>


void start_init_system(void) {
    size_t unused;
    char* start_code = elf_get_entry("initrd/init.sys", &unused);
    void(*entry)(void) = (void*)start_code;
    switch_to_ring3(entry);
}
