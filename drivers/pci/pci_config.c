#include <ioports.h>
#include <types.h>
#include <pci_config.h>
#include "pci_vendor.h"
#include <stdio.h>

#define CONFIG_ADDRESS	0xCF8
#define CONFIG_DATA	0xCFC

/* Retourne la valeur d'un registre pour une fonction données (ie. bus+slot+function) */
uint32_t pci_read_register(uint8_t bus,
			   uint8_t slot,
			   uint8_t func,
			   uint8_t reg)
{
	uint32_t reg_data = 0;
	uint32_t address;

	uint32_t dw_bus = (uint32_t) bus;
	uint32_t dw_slot = (uint32_t) slot;
	uint32_t dw_func = (uint32_t) func;
	uint32_t dw_reg = (uint32_t) reg; 

	address = (uint32_t)((dw_bus << 16)	| 
                             (dw_slot << 11)	|
              		     (dw_func << 8)	| 
                             (dw_reg & 0xfc)	|
                             ((uint32_t)0x80000000));

	outl(address, CONFIG_ADDRESS);
	reg_data = inl(CONFIG_DATA);

	return reg_data;
}

uint32_t pci_write_register(uint8_t bus,
			   uint8_t slot,
			   uint8_t func,
			   uint8_t reg,
			   uint32_t value)
{
	uint32_t address;

	uint32_t dw_bus = (uint32_t) bus;
	uint32_t dw_slot = (uint32_t) slot;
	uint32_t dw_func = (uint32_t) func;
	uint32_t dw_reg = (uint32_t) reg; 

	address = (uint32_t)((dw_bus << 16)	| 
                             (dw_slot << 11)	|
              		     (dw_func << 8)	| 
                             (dw_reg & 0xfc)	|
                             ((uint32_t)0x80000000));

	outl(address, CONFIG_ADDRESS);
	outl(value, CONFIG_DATA);
	return 0; /* TODO bonne valeur de retour */
}

uint32_t pci_read_value(pci_function_p func, uint8_t reg, uint8_t offset, uint32_t mask)
{
	uint32_t tmp_reg;

	tmp_reg = pci_read_register( func->bus, func->slot, func->function, reg);
	//printf("DATA:%x %x %x\n", offset, mask, tmp_reg);
	tmp_reg >>= offset;
	tmp_reg &= mask;
	//printf("OUT:%x\n", tmp_reg);

	return tmp_reg;

}

PPCI_VENTABLE pci_get_vendor(uint8_t bus, uint8_t slot, uint8_t function)
{
	uint16_t i;
	uint16_t vendor_id;
	PPCI_VENTABLE vendor = NULL;

	/* On récupère le vendor_id contenu dans le premier registre */
	vendor_id = pci_read_register( bus, slot, function, 0) & 0xffff;

	for(i=0; i<(uint16_t)PCI_VENTABLE_LEN; i++)
	{
		if(PciVenTable[i].VenId == vendor_id)
		{
			vendor = &PciVenTable[i];
			break;
		}
	}

	return vendor;
}

PPCI_DEVTABLE pci_get_device(uint8_t bus, uint8_t slot, uint8_t function)
{
	uint16_t i;
	uint16_t vendor_id;
	uint16_t device_id;
	uint32_t tmp_reg;
	PPCI_DEVTABLE device = NULL;

	/* On récupère le vendor_id contenu dans le premier registre */
	tmp_reg = pci_read_register( bus, slot, function, 0);
	vendor_id = tmp_reg & 0xffff;
	device_id = (tmp_reg >> 16) & 0xffff;

	for(i=0; i<(uint16_t)PCI_DEVTABLE_LEN; i++)
	{
		if(PciDevTable[i].VenId == vendor_id && PciDevTable[i].DevId == device_id)
		{
			device = &PciDevTable[i];
			break;
		}
	}

	return device;
}


PPCI_CLASSCODETABLE pci_get_classcode(uint8_t bus, uint8_t slot, uint8_t function)
{
	uint16_t i;
	uint8_t baseclass;
	uint8_t subclass;
	uint8_t progif;
	uint32_t tmp_reg;
	PPCI_CLASSCODETABLE classcode = NULL;
	
	/* On récupère le registre 0x08*/
	tmp_reg = pci_read_register( bus, slot, function, 0x08);

	/* Et on en extrait les valeurs classcode */
	baseclass = (uint8_t)(tmp_reg>>24);
	subclass = (uint8_t)((tmp_reg>>16)&0xff);
	progif = (uint8_t)((tmp_reg>>8)&0xff);

	for(i=0; i<(uint16_t)PCI_CLASSCODETABLE_LEN; i++)
	{
		if(PciClassCodeTable[i].BaseClass == baseclass && 
		   PciClassCodeTable[i].SubClass == subclass   && 
                   PciClassCodeTable[i].ProgIf == progif)
		{
			classcode = &PciClassCodeTable[i];
			break;
		}
	}
	
	return classcode;
}

void pci_print_info(pci_function_p func)
{
	printf("Bus %x, Slot %x, Func %x:\n",func->bus, func->slot, func->function);
	printf("    %s : %s(%s)\n",pci_get_vendor(func->bus,func->slot,func->function)->VenFull, 
                                  pci_get_device(func->bus,func->slot,func->function)->ChipDesc, 
                                  pci_get_device(func->bus,func->slot,func->function)->Chip);
}

void pci_print_detailed_info(pci_function_p func)
{
	uint32_t bar = 0;
	int irq;
	printf("Bus %x, Slot %x, Func %x:\n",func->bus, func->slot, func->function);
	printf("      Device: %s: %s (%s)\n",		pci_get_vendor(func->bus,func->slot,func->function)->VenFull,
											pci_get_device(func->bus,func->slot,func->function)->ChipDesc, 
											pci_get_device(func->bus,func->slot,func->function)->Chip);

	irq = pci_read_value(func, PCI_INTERRUPT_LINE);
	if(irq!=0)
		printf("      IRQ:%d\n", irq);
	
/*	printf("      Class: 0x%x 0x%x (0x%x)\n",pci_get_classcode(func->bus,func->slot,func->function)->SubClass,
   					       pci_get_classcode(func->bus,func->slot,func->function)->BaseClass,
                                             pci_get_classcode(func->bus,func->slot,func->function)->ProgIf);
*/
/*	printf("      Class: %s %s (%s)\n",pci_get_classcode(func->bus,func->slot,func->function)->BaseDesc,
   					       pci_get_classcode(func->bus,func->slot,func->function)->BaseDesc,
                                               pci_get_classcode(func->bus,func->slot,func->function)->ProgDesc);
*/
	bar = pci_read_value(func, PCI_BAR0);
	if(bar!=0)
		printf("      BAR0=0x%x\n",bar);

	bar = pci_read_value(func, PCI_BAR1);
	if(bar!=0)
		printf("      BAR1=0x%x\n",bar);

	bar = pci_read_value(func, PCI_BAR2);
	if(bar!=0)
		printf("      BAR2=0x%x\n",bar);

	bar = pci_read_value(func, PCI_BAR3);
	if(bar!=0)
		printf("      BAR0=3x%x\n",bar);

	bar = pci_read_value(func, PCI_BAR4);
	if(bar!=0)
		printf("      BAR4=0x%x\n",bar);

	bar = pci_read_value(func, PCI_BAR5);
	if(bar!=0)
		printf("      BAR0=0x%x\n",bar);

}

	

