#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <common/log.h>
#include <common/asm.h>
#include <common/init.h>
#include <intr/intr.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <mm/kheap.h>
#include <firmware/acpi/acpi.h>
#include <arch/x86/ioapic.h>
#include <arch/x86/lapic.h>
#include <arch/x86/gdt.h>
#include <timer/pit.h>
#include <proc/proc.h>
#include <proc/tss.h>
#include <fs/initrd.h>

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
    kheap_init();
    kprintf(STATUS_MSG_COLOR "[KHEAP]:" STATUS_MSG_OK_COLOR " OK\n"); 
    proc_init();
    kprintf(STATUS_MSG_COLOR "[PROC]:" STATUS_MSG_OK_COLOR " OK\n");
    initrd_init();
    kprintf(STATUS_MSG_COLOR "[INITRD]:" STATUS_MSG_OK_COLOR " OK\n");
    CLI;
    load_gdt();
    kprintf(STATUS_MSG_COLOR "[GDT]:" STATUS_MSG_OK_COLOR " OK\n");
    write_tss();
    load_tss();
    kprintf(STATUS_MSG_COLOR "[TSS]:" STATUS_MSG_OK_COLOR " OK\n");
    // STI;

    start_init_system();

    done();
}

void _start(void) {
    CLI;
	init();
}
