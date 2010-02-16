#ifndef _PCI_CONFIG_H
#define _PCI_CONFIG_H

#include <pci_types.h>

#define PCI_DEVICE_ID		0x00, 2
#define PCI_VENDOR_ID		0x02, 2
#define PCI_STATUS		0x04, 2
#define PCI_COMMAND		0x06, 2
#define PCI_CLASS_CODE		0x08, 1
#define PCI_SUBCLASS		0x09, 1
#define PCI_PROG_IF		0x0A, 1
#define PCI_REVISION_ID		0x0B, 1
#define PCI_BIST		0x0C, 1
#define PCI_HEADER_TYPE		0x0D, 1
#define PCI_LATENCY_TIMER	0x0E, 1
#define PCI_CACHE_LINE_SIZE	0x0F, 1
#define PCI_BAR0		0x10, 4
#define PCI_BAR1		0x14, 4
#define PCI_BAR2		0x18, 4
#define PCI_BAR3		0x1C, 4
#define PCI_BAR4		0x20, 4
#define PCI_BAR5		0x24, 4
#define PCI_CARDBUS_CIS_POINTER	0x28, 4
#define PCI_SUBSYS_ID		0x2C, 2 
#define PCI_SUBSYS_VENDOR_ID	0x2E, 2		
#define PCI_EXPROM_BAR		0x30, 4
#define PCI_CAP_POINTER		0x37, 1
#define PCI_MAX_LATENCY		0x3c, 1
#define PCI_MIN_GRANT		0x3d, 1
#define PCI_INTERRUPT_PIN	0x3e, 1
#define PCI_INTERRUPT_LINE	0x3f, 1

uint32_t pci_read_register(uint8_t bus, uint8_t device, uint8_t func, uint8_t reg);

uint32_t pci_read_value( uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint8_t size);

void pci_print_info( pci_function_p function);


#endif
