#include <bus/pci/class.h>

const char* const PCI_CLASS_STRINGS[] = {
    [PCI_BC_NULL] = "PCI_BC_NULL",
    [PCI_BC_MASS_STORAGE_CONTROLLER] = "MASS STORAGE CONTROLLER",
    [PCI_BC_NETWORK_CONTROLLER] = "NETWORK CONTROLLER",
    [PCI_BC_DISPLAY_CONTROLLER] = "DISPLAY CONTROLLER",
    [PCI_BC_WIRELESS_CONTROLLER] = "WIRELESS CONTROLLER",
    [PCI_BC_ERR] = "ERR"
};

uint8_t pci_is_class_used(uint16_t class) {
    switch (class) {
        case PCI_BC_MASS_STORAGE_CONTROLLER:
        case PCI_BC_NETWORK_CONTROLLER:
        case PCI_BC_DISPLAY_CONTROLLER:
        case PCI_BC_WIRELESS_CONTROLLER:
            return 1;
        default:
            return 0;
    }
}
