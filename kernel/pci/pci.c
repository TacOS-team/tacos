/**
 * @file pci.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012 - TacOS developers.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * @section DESCRIPTION
 *
 * Description de ce que fait le fichier
 */

#include <pci.h>
#include <pci_config.h>
#include <pci_types.h>
#include <types.h>
#include <klog.h>
#include <kstdio.h>

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

pci_function_p pci_find_device( uint16_t vendor_id, uint16_t device_id )
{
	pci_function_p func = NULL;
	int i;
	int vendor, device;
	
	for(i=0; (i<pci_table_len) && (func == NULL); i++)
	{	
		vendor = pci_read_value(&pci_table[i], PCI_VENDOR_ID);
		device = pci_read_value(&pci_table[i], PCI_DEVICE_ID);
		if(vendor == vendor_id && device == device_id)
			func = &pci_table[i];
	}
	
	return func;
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

	klog("Scanning pci bus...");
	
	/* On scan les 256 bus possibles */
	for(bus = 0; bus<10; bus++)
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
	if(pci_table_len>0)
	{
		klog("%d device(s) found.",pci_table_len);
	} else {
		klog("no device found.\n");
	}
}


void pci_list()
{
	uint16_t i=0;

	for(i=0; i<pci_table_len; i++)
	{
		pci_print_detailed_info(pci_get_function(i));
		//pci_print_info(pci_get_function(i));
	}
}






 
			
	
	

