/**
 * @file pci.h
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2015 TacOS developers.
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

#ifndef _PCI_H_
#define _PCI_H_

#include <types.h>
#include <pci_types.h>

typedef unsigned short int pci_device_t;

pci_function_p pci_find_device( uint16_t vendor_id, uint16_t device_id );

uint8_t pci_get_device_bus( pci_device_t device );
uint8_t pci_get_device_slot( pci_device_t device );
uint8_t pci_get_device_function_nb( pci_device_t device );

void pci_scan();
void pci_list();

#endif // _PCI_H_
