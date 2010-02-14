#include <ioports.h>
#include <pci_vendor.h>
#include <stdio.h>
#include <types.h>

#define CONFIG_ADDRESS	0xCF8
#define CONFIG_DATA	0xCFC

#define PCI_MAX_DEVICES 256

struct pci_device_descriptor
{
	uint8_t bus;
	uint8_t device;

	uint8_t function_nb;
};

/* Table contenant les descripteurs des périphériques pci identifiés */
static struct pci_device_descriptor pci_table[PCI_MAX_DEVICES];

/* taille de pci_table */
static uint16_t pci_table_len = 0;


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

	printf("Scanning pci bus...");
	
	/* On scan les 256 bus possibles */
	for(bus = 0; bus<256; bus++)
	{
		/* les 32 périphériques maximum par bus */
		for(device = 0; device<32; device++)
		{
			tmp_reg = pci_read_register((uint16_t)bus, device, 0, 0);
			tmp_vendor = (uint16_t)(tmp_reg & 0xffff);
			if(tmp_vendor!=0xffff)
			{
				pci_table_len++;
				/* On ajoute le périphérique trouvé à la table */
				pci_table[pci_table_len - 1].bus = bus;
				pci_table[pci_table_len - 1].device = bus;
				pci_table[pci_table_len - 1].function_nb = 0;

				/* Puis on compte les fonctions proposées par le périphérique (jusqu'a 8 par périph) */
				for(func = 0; func<8; func++)
				{
					tmp_reg = pci_read_register((uint16_t)bus, device, func, 0);
					tmp_vendor = (uint16_t)(tmp_reg & 0xffff);
					if(tmp_vendor!=0xffff)
					{
						pci_table[pci_table_len - 1].function_nb++;
					}
				}
			}
		}
	}
	pci_table_len>0?printf("%d device(s) found.\n",pci_table_len):printf("no device found.\n");;
}



 
			
	

