#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <common/log.h>
#include <common/asm.h>
#include <common/init.h>
#include <common/font.h>
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
#include <bus/pci/pci.h>
#include <uapi/input/keyboard.h>
#include <uapi/video/screen.h>
#include <uapi/proc/info.h>
#include <drivers/video/lfb.h>


static void uapi_init(void) {
    keyboard_uapi_init();
    screen_uapi_init();
    procinfo_uapi_init();
}


static void __attribute__((noreturn)) init(void) {
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
   
    kprintf(STATUS_MSG_COLOR "[SYSTEM]:" YELLOW " Starting PCI dump (will be truncated).\n\n");
    pci_dump();
    
    // Ensure the built in font is loaded.
    init_font();

    // Clear screen.
    lfb_clear_screen(0x000000);
    
    uapi_init();
    start_init_system();
    while (1);
}

void _start(void) {
    CLI;
	init();
}
