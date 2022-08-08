#include <firmware/acpi/acpi.h>
#include <firmware/acpi/tables.h>
#include <common/asm.h>
#include <common/log.h>
#include <common/panic.h>
#include <common/string.h>
#include <limine.h>

#define APIC_TYPE_LOCAL_APIC            0
#define APIC_TYPE_IO_APIC               1
#define APIC_TYPE_INTERRUPT_OVERRIDE    2


static volatile struct limine_rsdp_request rsdp_req = {
	.id = LIMINE_RSDP_REQUEST,
	.revision = 0
};


static acpi_rsdt_t* rsdt = NULL;
static acpi_madt_t* madt = NULL;
static uint32_t rsdt_entries = 0;
static uint8_t n_cores = 0;

// MMIO base address for Local APIC and I/O APIC.
static void* lapic_base = NULL;
static void* ioapic_base = NULL;


static void fetch_madt(void) {
	// Loop through all RSDT entries until
	// we find that one of these entrie's header
	// has the signature of APIC.
	for (uint32_t i = 0; i < rsdt_entries; ++i) {
		acpi_header_t* cur = (acpi_header_t*)(uint64_t)rsdt->tables[i];
		if (strncmp(cur->signature, "APIC", 4) == 0) {
			madt = (acpi_madt_t*)cur;
			return;
		}
	}
}

static void parse_madt(void) {
	lapic_base = (void*)(uint64_t)(madt->lapic_addr);
 	uint8_t* cur = (uint8_t*)(madt + 1);
    uint8_t* end = (uint8_t*)madt + madt->header.length;

	while (cur < end) {
		apic_header_t* apic_header = (apic_header_t*)cur;

		switch (apic_header->type) {
			case APIC_TYPE_LOCAL_APIC:
				++n_cores;
				break;
			case APIC_TYPE_IO_APIC:
				{
					io_apic_t* ioapic = (io_apic_t*)cur;
					ioapic_base = (void*)(uint64_t)ioapic->io_apic_addr;
				}
				break;
		}

		cur += apic_header->length;
	}
}


void* acpi_get_lapic_base(void) {
	return lapic_base;
}

void* acpi_get_ioapic_base(void) {
	return ioapic_base;
}

uint8_t acpi_get_core_count(void) {
	return n_cores;
}


uint8_t verify_checksum(acpi_header_t* header) {
	uint8_t sum = 0;

	for (uint32_t i = 0; i < header->length; ++i) {
		sum += ((char*)header)[i];
	}

	return sum % 0x100 == 0;
}


void acpi_init(void) {
	acpi_rsdp_t* rsdp = rsdp_req.response->address;
	rsdt = (acpi_rsdt_t*)(uint64_t)rsdp->rsdtaddr;
    rsdt_entries = (rsdt->header.length - sizeof(rsdt->header)) / 4;

	if (!(verify_checksum(&rsdt->header))) {
		kprintf(KERN_PANIC "RSDT checksum is invalid.\n");
		CLI; HLT;
	}

	fetch_madt();
	if (madt == NULL) {
		kprintf(KERN_PANIC "MADT not found!\n");
		CLI; HLT;
	}

	if (!(verify_checksum(&madt->header))) {
		kprintf(KERN_PANIC "MADT checksum invalid!\n");
		CLI; HLT;
	}

	// Parse the MADT.
	parse_madt();
}
