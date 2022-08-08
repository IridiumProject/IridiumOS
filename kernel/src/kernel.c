#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <common/log.h>
#include <intr/intr.h>
#include <mm/pmm.h>
#include <mm/vmm.h>

#define STATUS_MSG_COLOR CYAN
#define STATUS_MSG_OK_COLOR KINFO


static void done(void) {
    for (;;) {
        __asm__("hlt");
    }
}

void _start(void) {
    intr_init();
    kprintf(STATUS_MSG_COLOR "[Interrupts Init]:" STATUS_MSG_OK_COLOR " OK\n");
    pmm_init();
    kprintf(STATUS_MSG_COLOR "[PMM]:" STATUS_MSG_OK_COLOR " OK\n");
    vmm_init();
    kprintf(STATUS_MSG_COLOR "[VMM]:" STATUS_MSG_OK_COLOR " OK\n");
	kprintf(STATUS_MSG_COLOR "[ACPI]:" STATUS_MSG_OK_COLOR " OK\n");
    done();
}
