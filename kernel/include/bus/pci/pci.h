#ifndef PCI_H
#define PCI_H

#include <stdint.h>
#include <bus/pci/class.h>

#define PCI_IS_VENDOR_VALID(bus, slot, func) pci_read_vendor(bus, slot, func) != 0xFFFF
#define PCI_MAX_BUS 256
#define PCI_MAX_SLOTS 32
#define PCI_MAX_FUNCS 8


uint16_t pci_read_vendor(uint8_t bus, uint8_t slot, uint8_t func);
uint16_t pci_read_dev_id(uint8_t bus, uint8_t slot, uint8_t func);
uint16_t pci_read_rev_id(uint8_t bus, uint8_t slot, uint8_t func);
uint16_t pci_read_progif(uint8_t bus, uint8_t slot, uint8_t func);
uint16_t pci_read_subclass(uint8_t bus, uint8_t slot, uint8_t func);
uint16_t pci_read_class(uint8_t bus, uint8_t slot, uint8_t func);
void pci_dump(void);


#endif
