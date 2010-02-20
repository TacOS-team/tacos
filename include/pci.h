#ifndef _PCI_H
#define _PCI_H
#include <types.h>

typedef unsigned short int pci_device_t;

uint8_t pci_get_device_bus( pci_device_t device );
uint8_t pci_get_device_slot( pci_device_t device );
uint8_t pci_get_device_function_nb( pci_device_t device );

void pci_scan();

#endif
