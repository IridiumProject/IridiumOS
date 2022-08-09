#ifndef ACPI_H
#define ACPI_H

#include <stdint.h>


void acpi_init(void);
void* acpi_get_lapic_base(void);
void* acpi_get_ioapic_base(void);
uint8_t acpi_get_core_count(void);

/*
 *	Converts an IRQ number to a 
 *	Global System Interrupt.
 */

uint16_t acpi_irq_to_gsi(uint8_t irq);

#endif
