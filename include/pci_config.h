#ifndef _PCI_CONFIG_H
#define _PCI_CONFIG_H

#include <pci_types.h>

#define PCI_DEVICE_ID		0x00, 16, 0xffff
#define PCI_VENDOR_ID		0x00,  0, 0xffff 
#define PCI_STATUS		0x04, 16, 0xffff
#define PCI_COMMAND		0x04,  0, 0xffff
#define PCI_CLASS_CODE		0x08, 24, 0x00ff
#define PCI_SUBCLASS		0x08, 16, 0x00ff
#define PCI_PROG_IF		0x08,  8, 0x00ff
#define PCI_REVISION_ID		0x08,  0, 0x00ff
#define PCI_BIST		0x0C, 24, 0x00ff
#define PCI_HEADER_TYPE		0x0C, 16, 0x00ff
#define PCI_LATENCY_TIMER	0x0C,  8, 0x00ff
#define PCI_CACHE_LINE_SIZE	0x0C,  0, 0x00ff
#define PCI_BAR0		0x10,  0, 0xffffffff
#define PCI_BAR1		0x14,  0, 0xffffffff
#define PCI_BAR2		0x18,  0, 0xffffffff
#define PCI_BAR3		0x1C,  0, 0xffffffff
#define PCI_BAR4		0x20,  0, 0xffffffff
#define PCI_BAR5		0x24,  0, 0xffffffff
#define PCI_CARDBUS_CIS_POINTER	0x28,  0, 0xffffffff
#define PCI_SUBSYS_ID		0x2C, 16, 0xffff 
#define PCI_SUBSYS_VENDOR_ID	0x2C,  0, 0xffff		
#define PCI_EXPROM_BAR		0x30,  0, 0xffffffff
#define PCI_CAP_POINTER		0x34,  0, 0x00ff
#define PCI_MAX_LATENCY		0x3c, 24, 0x00ff
#define PCI_MIN_GRANT		0x3c, 16, 0x00ff
#define PCI_INTERRUPT_PIN	0x3c,  8, 0x00ff
#define PCI_INTERRUPT_LINE	0x3c,  0, 0x00ff

uint32_t pci_read_register(uint8_t bus, uint8_t device, uint8_t func, uint8_t reg);

uint32_t pci_read_value(pci_function_p func, uint8_t reg, uint8_t offset, uint32_t mask);

uint32_t pci_write_register(uint8_t bus, uint8_t slot, uint8_t func, uint8_t reg, uint32_t value);

void pci_print_info( pci_function_p function);


#endif
