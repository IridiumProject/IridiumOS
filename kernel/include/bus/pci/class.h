#ifndef PCI_DEF_H
#define PCI_DEF_H

#include <stdint.h>


// NOTE: Update is_class_used() if updating this enum.
typedef enum {
    PCI_BC_NULL = 0x0,
    PCI_BC_MASS_STORAGE_CONTROLLER = 0x1,
    PCI_BC_NETWORK_CONTROLLER = 0x2,
    PCI_BC_DISPLAY_CONTROLLER = 0x3,
    PCI_BC_WIRELESS_CONTROLLER = 0xD,
    PCI_BC_ERR = 0xFF
} PCI_BASE_CLASSES;


typedef enum {
    PCITYPE_SCSI_DEVICE,
    PCITYPE_IDE_CONTROLLER,
    PCITYPE_FLOPPY_DISK_CONTROLLER,
    PCITYPE_IPI_BUS_CONTROLLER,
    PCITYPE_IPI_RAID_CONTROLLER,
    PCITYPE_ATA_CONTROLLER,
    PCITYPE_SATA_CONTROLLER,
    PCITYPE_NVM,
    PCITYPE_UFS,
    PCITYPE_OTHER_MS = 0x80,              // OTHER MASS STORAGE.
} PCI_MASS_STORAGE;


typedef enum {
    PCITYPE_ETHERNET_CONTROLLER = 0,
    PCITYPE_ATM_CONTROLLER = 3,
    PCITYPE_OTHER_NW = 0x80                     // Other Network.
} PCI_BC_NETWORKING;

typedef enum {
    PCITYPE_VGA = 0,
    PCITYPE_XGA = 1,
    PCITYPE_3D = 2,
    PCITYPE_OTHER_DISPLAY = 0x80
} PCI_BC_DISPLAY;


typedef enum {
    PCITYPE_IRDA = 0,
    PCI_RF_CONTROLLER = 16,
    PCI_BROADBAND = 18
} PCI_BC_WIRELESS;

/*
 *  Returns 1 if the class is useful to IridiumOS.
 *
 */
uint8_t pci_is_class_used(uint16_t class);

extern const char* const PCI_CLASS_STRINGS[];

#endif
