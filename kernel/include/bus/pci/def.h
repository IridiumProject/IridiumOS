#ifndef PCI_DEF_H
#define PCI_DEF_H


typedef enum {
    PCI_BC_NULL,
    PCI_BC_MASS_STORAGE_CONTROLLER,
    PCI_BC_NETWORK_CONTROLLER,
    PCI_BC_DISPLAY_CONTROLLER,
    PCI_BC_MULTIMEDIA_DEVICE,
    PCI_BC_MEMORY_CONTROLLER,
    PCI_BC_BRIDGE_DEVICE,
    PCI_BC_SCC,                     // Simple communication controllers.
    PCI_BC_BSP,                     // Base system peripherals.
    PCI_BC_INPUT_DEVICES,
    PCI_BC_DOCKING_STATIONS,
    PCI_BC_PROCESSORS,
    PCI_BC_SERIAL_BUS_CONTROLLERS,
    PCI_BC_WIRELESS_CONTROLLER,
    PCI_BC_IIO,                     // Intelligent I/O controllers.
    PCI_BC_SAT_COMM_CONTROLLER,     // Satellite communication controllers.
    PCI_BC_EDC,                     // Encryption/Decryption controllers.
    PCI_BC_DASP_CONTROLLERS,        // Data acquisition and signal processing controllers.
    PCI_BC_ERR = 0xFF
} PCI_BASE_CLASSES;


typedef enum {

} PCI_BC_01;


#endif
