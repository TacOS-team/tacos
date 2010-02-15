#include <ioports.h>
#include <types.h>
#include <pci.h>
#include <pci_vendor.h>

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

uint32_t pci_read_value(pci_device_t device, uint8_t function, uint8_t offset, uint8_t size)
{
	uint32_t reg;

	reg = pci_read_register( pci_get_device_bus(device), pci_get_device_slot(device), function, (offset & 0xfc));
	
	/* Formula from hell */
	reg >>= (4-(offset & 3)-size)*4;
	reg &= (0xffff>>((4-size)*16));

	return reg;

}

PPCI_VENTABLE pci_get_vendor(pci_device_t device, uint8_t function)
{
	uint16_t i;
	uint16_t vendor_id;
	PPCI_VENTABLE vendor = NULL;

	/* On récupère le vendor_id contenu dans le premier registre */
	vendor_id = pci_read_register( pci_get_device_bus(device), pci_get_device_slot(device), function, 0) & 0xffff;

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

PPCI_CLASSCODETABLE pci_get_classcode(pci_device_t device, uint8_t function)
{
	uint16_t i;
	uint8_t baseclass;
	uint8_t subclass;
	uint8_t progif;
	uint32_t tmp_reg;
	PPCI_CLASSCODETABLE classcode = NULL;
	
	/* On récupère le registre 0x08*/
	tmp_reg = pci_read_register( pci_get_device_bus(device), pci_get_device_slot(device), function, 0x08);

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

