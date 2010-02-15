#include <pci.h>
#include <pci_config.h>
#include <stdio.h>
#include <types.h>

#define PCI_MAX_DEVICES 256

struct pci_device_descriptor
{
	uint8_t bus;
	uint8_t slot;

	uint8_t function_nb;
};

/* Table contenant les descripteurs des périphériques pci identifiés */
static struct pci_device_descriptor pci_table[PCI_MAX_DEVICES];

/* taille de pci_table */
static uint16_t pci_table_len = 0;


/* Procedures d'accès aux données de pci_device_descriptor */
/** TODO : sécuriser les acces (vérifier les débordements de tableau par exemple **/
uint8_t pci_get_device_bus( pci_device_t device )
{
	return pci_table[device].bus;
}

uint8_t pci_get_device_slot( pci_device_t device )
{
	return pci_table[device].bus;
}

uint8_t pci_get_device_function_nb( pci_device_t device )
{
	return pci_table[device].function_nb;
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
	uint16_t tmp_slot;	

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
				pci_table_len++;
				/* On ajoute le périphérique trouvé à la table */
				pci_table[pci_table_len - 1].bus = bus;
				pci_table[pci_table_len - 1].slot = bus;
				pci_table[pci_table_len - 1].function_nb = 0;

				/* Puis on compte les fonctions proposées par le périphérique (jusqu'a 8 par périph) */
				for(func = 0; func<8; func++)
				{
					tmp_reg = pci_read_register((uint16_t)bus, slot, func, 0);
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






 
			
	

