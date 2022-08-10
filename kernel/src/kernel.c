#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <common/log.h>
#include <common/asm.h>
#include <intr/intr.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <firmware/acpi/acpi.h>
#include <arch/x86/ioapic.h>
#include <arch/x86/lapic.h>
#include <timer/pit.h>

static void done(void) {
    for (;;) {
        __asm__("hlt");
    }
}


static void init(void) {
    intr_init();
    kprintf(STATUS_MSG_COLOR "[Interrupts Init]:" STATUS_MSG_OK_COLOR " OK\n");
    pmm_init();
    kprintf(STATUS_MSG_COLOR "[PMM]:" STATUS_MSG_OK_COLOR " OK\n");
    vmm_init();
    kprintf(STATUS_MSG_COLOR "[VMM]:" STATUS_MSG_OK_COLOR " OK\n");
	acpi_init();
	kprintf(STATUS_MSG_COLOR "[ACPI]:" STATUS_MSG_OK_COLOR " OK\n");
	ioapic_init();
	kprintf(STATUS_MSG_COLOR "[I/O APIC]:" STATUS_MSG_OK_COLOR " OK\n");
	lapic_init();
	kprintf(STATUS_MSG_COLOR "[Local APIC]:" STATUS_MSG_OK_COLOR " OK\n");
	pit_init();
	kprintf(STATUS_MSG_COLOR "[PIT]:" STATUS_MSG_OK_COLOR " OK\n");
	init_irqs();
	kprintf(STATUS_MSG_COLOR "[IRQS]:" STATUS_MSG_OK_COLOR " OK\n");
	STI;
    done();
}

void _start(void) {
	init();
}
