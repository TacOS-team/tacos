#ifndef _PCI_H_
#define _PCI_H_

#include <types.h>

typedef unsigned short int pci_device_t;

uint8_t pci_get_device_bus( pci_device_t device );
uint8_t pci_get_device_slot( pci_device_t device );
uint8_t pci_get_device_function_nb( pci_device_t device );

void pci_scan();
void pci_list();

#endif // _PCI_H_
