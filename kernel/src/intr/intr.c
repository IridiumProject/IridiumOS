#include <intr/intr.h>
#include <intr/IDT.h>
#include <intr/exceptions.h>
#include <intr/irq.h>
#include <intr/syscall.h>
#include <firmware/acpi/acpi.h>
#include <arch/x86/ioapic.h>
#include <stdint.h>

#define TRAP_GATE_FLAGS 0x8F
#define INT_GATE_FLAGS 0x8E
#define IDT_INT_GATE_USER 0xEE

static void(*exceptions[])(void) = {
    divide_error,
    debug_exception,
    general_protection_fault,
    general_protection_fault,
    overflow,
    bound_range_exceeded,
    invalid_opcode,
    no_mathcoprocessor,
    double_fault,
    general_protection_fault,
    invalid_tss,
    segment_not_present,
    stack_segment_fault,
    general_protection_fault,
    page_fault
};


static void init_exceptions(void) {
    for (uint16_t i = 0; i <= 0xE; ++i) {
        idt_set_desc(i, exceptions[i], TRAP_GATE_FLAGS);
    }
}


void init_irqs(void) {
	// Timer.
	idt_set_desc(0x20, irq0_isr, INT_GATE_FLAGS);
	ioapic_set_entry(acpi_irq_to_gsi(0), 0x20);
}


void intr_init(void) {
    idt_set_desc(0x80, dispatch_syscall, IDT_INT_GATE_USER);
    init_exceptions();
    idt_install();
}
