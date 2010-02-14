#include <ioports.h>
#include <pci_vendor.h>
#include <stdio.h>
#include <types.h>

#define CONFIG_ADDRESS	0xCF8
#define CONFIG_DATA	0xCFC

uint32_t pci_read_register(uint8_t bus,
			   uint8_t device,
			   uint8_t func,
			   uint8_t reg)
{
	uint32_t reg_data = 0;
	uint32_t address;

	uint32_t dw_bus = (uint32_t) bus;
	uint32_t dw_device = (uint32_t) device;
	uint32_t dw_func = (uint32_t) func;
	uint32_t dw_reg = (uint32_t) reg; 

	address = (uint32_t)((dw_bus << 16)	| 
                             (dw_device << 11)	|
              		     (dw_func << 8)	| 
                             (dw_reg & 0xfc)	|
                             ((uint32_t)0x80000000));
	outl(address, CONFIG_ADDRESS);
	reg_data = inl(CONFIG_DATA);
	return reg_data;
}

PPCI_VENTABLE pci_get_vendor(uint16_t vendor_id)
{
	uint16_t i;
	PPCI_VENTABLE vendor = NULL;

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

PPCI_CLASSCODETABLE pci_get_classcode(uint8_t baseclass, 
				      uint8_t subclass, 
				      uint8_t progif)
{
	uint16_t i;
	PPCI_CLASSCODETABLE classcode = NULL;
	
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

void pci_scan()
{
	uint16_t bus;
	uint8_t device;
	uint8_t func;
	//uint8_t reg;

	uint32_t tmp_reg;
	uint16_t tmp_vendor;
	uint16_t tmp_device;
	uint8_t tmp_baseclass; 
	uint8_t tmp_subclass; 
	uint8_t tmp_progif;

	printf("Scanning pci bus...\n");
	for(bus = 0; bus<256; bus++)
	{
		for(device = 0; device<32; device++)
		{
			tmp_reg = pci_read_register((uint16_t)bus, device, 0, 0);
			tmp_vendor = (uint16_t)(tmp_reg & 0xffff);
			if(tmp_vendor!=0xffff)
			{
				for(func = 0; func<8; func++)
				{
					tmp_reg = pci_read_register((uint16_t)bus, device, func, 0);
					tmp_vendor = (uint16_t)(tmp_reg & 0xffff);
					if(tmp_vendor!=0xffff)
					{
						tmp_device = (uint16_t)((tmp_reg >> 16) & 0xffff);

						tmp_reg = pci_read_register((uint16_t)bus, device, func, 8);
						tmp_baseclass = (uint8_t)((tmp_reg>>24)&0xff);
						tmp_subclass = (uint8_t)((tmp_reg>>16)&0xff);
						tmp_progif = (uint8_t)((tmp_reg>>8)&0xff);
						//pintf("Bus %d Device %d Function %d:\n", bus, device, func);
						printf("    %s : %s %s\n",pci_get_vendor(tmp_vendor)->VenFull,
                                                                            pci_get_classcode(tmp_baseclass, 
                                                                                              tmp_subclass, 
                                                                                              tmp_progif)->SubDesc,
									    pci_get_classcode(tmp_baseclass, 
                                                                                              tmp_subclass, 
                                                                                              tmp_progif)->BaseDesc);
					}
				}
			}
		}
	}
	printf("Scanning done.\n");
}



 
			
	

