#include <intr/intr.h>
#include <intr/IDT.h>
#include <intr/exceptions.h>
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


void intr_init(void) {
    init_exceptions();
    idt_install();
}
