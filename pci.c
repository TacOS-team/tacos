#include <pci.h>
#include <pci_config.h>
#include <pci_types.h>
#include <stdio.h>
#include <types.h>

#define PCI_MAX_DEVICES 256


/* Table contenant les descripteurs des périphériques pci identifiés */
static pci_function_t pci_table[PCI_MAX_DEVICES];

/* taille de pci_table */
static uint16_t pci_table_len = 0;


/* Procedures d'accès aux données de pci_device_descriptor */
pci_function_p pci_get_function( pci_desc_t desc )
{
	if(desc >= pci_table_len)
		return NULL;
	else
		return &(pci_table[desc]);
}

/* Scan le bus pci et ajoute les périphériques trouvés à pci_table */
void pci_scan()
{
	uint16_t bus;
	uint8_t slot;
	uint8_t func;
	//uint8_t reg;

	uint32_t tmp_reg;
	uint16_t tmp_vendor;
	//uint16_t tmp_slot;	

	printf("Scanning pci bus...");
	
	/* On scan les 256 bus possibles */
	for(bus = 0; bus<256; bus++)
	{
		/* les 32 périphériques maximum par bus */
		for(slot = 0; slot<32; slot++)
		{
			tmp_reg = pci_read_register((uint16_t)bus, slot, 0, 0);
			tmp_vendor = (uint16_t)(tmp_reg & 0xffff);
			if(tmp_vendor!=0xffff)
			{
				
				/* Puis on compte les fonctions proposées par le périphérique (jusqu'a 8 par périph) */
				for(func = 0; func<8; func++)
				{
					tmp_reg = pci_read_register((uint16_t)bus, slot, func, 0);
					tmp_vendor = (uint16_t)(tmp_reg & 0xffff);
					if(tmp_vendor!=0xffff)
					{
						pci_table_len++;
						/* On ajoute le périphérique trouvé à la table */
						pci_table[pci_table_len - 1].bus = bus;
						pci_table[pci_table_len - 1].slot = slot;
						pci_table[pci_table_len - 1].function = func;

					}
				}
			}
		}
	}
	pci_table_len>0?printf("%d device(s) found.\n",pci_table_len):printf("no device found.\n");;
}


void pci_list()
{
	uint16_t i;

	for(i=0; i<pci_table_len; i++)
	{
		pci_print_detailed_info(pci_get_function(i));
	}
}






 
			
	

