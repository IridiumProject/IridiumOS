#include <arch/x86/ioapic.h>
#include <firmware/acpi/acpi.h>
#include <common/debug.h>
#ifdef DEBUG
#include <common/log.h>
#endif
#include <stddef.h>

// Memory mapped register for I/O APIC access.
#define IOREGSEL                        0x00
#define IOWIN                           0x10

// Other I/O APIC registers.
#define IOAPICID                        0x00
#define IOAPICVER                       0x01
#define IOAPICARB                       0x02
#define IOREDTBL                        0x10

static void* ioapic_base = NULL;


static uint32_t read(uint8_t reg) {
	*(volatile uint32_t*)ioapic_base = reg;
	return *(volatile uint32_t*)(ioapic_base + IOWIN);
}


static void write(uint8_t reg, uint32_t value) {
	*(volatile uint32_t*)ioapic_base = reg;
	*(volatile uint32_t*)(ioapic_base + IOWIN) = value;
}


void ioapic_set_entry(uint8_t index, uint64_t data) {
	write(IOREDTBL + index * 2, (uint32_t)data);
	write(IOREDTBL + index * 2 + 1, (uint32_t)(data >> 32));
}

void ioapic_init(void) {
	ioapic_base = acpi_get_ioapic_base();
	uint16_t n_pins = ((read(IOAPICVER) >> 16) & 0xFF) + 1;

#ifdef DEBUG
	kprintf(STATUS_MSG_COLOR "<debug>: I/O APIC pin count: %d\n", n_pins);
#endif

	// Mask entries.
	for (uint16_t i = 0; i < n_pins; ++i) {
		ioapic_set_entry(i, 1 << 16);
	}


#ifdef DEBUG
		kprintf(STATUS_MSG_COLOR "<debug>: Masked all REDENTREIS.\n");
#endif
}
