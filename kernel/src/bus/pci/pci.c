#include <bus/pci/pci.h>
#include <common/log.h>
#include <arch/io.h>


static uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    const uint16_t CONFIG_ADDRESS = 0xCF8;
    const uint16_t CONFIG_DATA = 0xCFC;

    uint32_t addr = (uint32_t)((lbus << 16) | (slot << 11) | (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
    outportl(CONFIG_ADDRESS, addr);
    return (uint16_t)((inportl(CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);
}


uint16_t pci_read_vendor(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_config_read_word(bus, slot, func, 0x0);
}


uint16_t pci_read_dev_id(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_config_read_word(bus, slot, func, 0x2);
}


uint16_t pci_read_rev_id(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_config_read_word(bus, slot, func, 0x8) & 0xFF;
}


uint16_t pci_read_progif(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_config_read_word(bus, slot, func, 0x8) >> 8;
}

uint16_t pci_read_subclass(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_config_read_word(bus, slot, func, 0xA) & 0xFF;
}

uint16_t pci_read_class(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_config_read_word(bus, slot, func, 0xA) >> 8;
}

void pci_dump(void) {
    for (uint16_t bus = 0; bus < PCI_MAX_BUS; ++bus) {
        for (uint8_t slot = 0; slot < PCI_MAX_SLOTS; ++slot) {
            for (uint8_t func = 0; func < PCI_MAX_FUNCS; ++func) {
                uint16_t class = pci_read_class(bus, slot, func);

                if (!(PCI_IS_VENDOR_VALID(bus, slot, func)) || !(pci_is_class_used(class))) {
                    continue;
                }

                kprintf(STATUS_MSG_COLOR "[PCI]: " BOLD_YELLOW "Device with vendor ID %x found\n", pci_read_vendor(bus, slot, func));
                kprintf(STATUS_MSG_COLOR "[PCI]: " BOLD_YELLOW "Device type: %s\n\n", PCI_CLASS_STRINGS[class]);
            }
        }
    }
}
