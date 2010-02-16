#ifndef _PCI_TYPES_H_
#define _PCI_TYPES_H_

#include <types.h>

typedef unsigned short int pci_desc_t;

typedef struct
{
	uint8_t bus;
	uint8_t slot;
	uint8_t function;

}pci_function_t, *pci_function_p;


#endif
