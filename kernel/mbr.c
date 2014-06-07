/**
 * @file mbr.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2014 TacOS developers.
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

#include <floppy.h>
#include <types.h>
#include "mbr.h"

typedef struct _PartitionDescriptor {
	
	uint8_t active;
	uint8_t firstHead;
	uint8_t firstSector; // 6bits : n'utilise que les 6bits de poids faible de l'octet d'offset 0x02 du descripteur
	uint16_t firstCylinder; // 10bits: utilise les 2bits de poid for de l'octet d'offset 0x02 et les 8bits de de l'octet d'offset 0x03 du descripteur
	uint8_t type;
	uint8_t lastHead;
	uint8_t lastSector; // 6bits : n'utilise que les 6bits de poids faible de l'octet d'offset 0x06 du descripteur
	uint16_t lastCylinder; // 10bits: utilise les 2bits de poid for de l'octet d'offset 0x06 et les 8bits de de l'octet d'offset 0x07 du descripteur	
	uint32_t LBA; // Logical Block Addressing du premier secteur, soit numero (en numerotation absolu sur le disque) du premier secteur de la partition
	uint32_t nbBlocks; // nombre de block qu'occupe la partition
	
} PartitionDescriptor;

uint8_t MasterBootRecord[512];

PartitionDescriptor PartitionTable[4];


void mbr_read () {
	
	int i;
	int k;
	
	floppy_read_sector(0, 0, 0, (char*) MasterBootRecord);
	
	k = 0x1BE;
	for (i=0;i<4;i++) {
		PartitionTable[i].active = 0xFF&MasterBootRecord[k+0x00];
		PartitionTable[i].firstHead = 0xFF&MasterBootRecord[k+0x01];
		PartitionTable[i].firstSector = 0x3F&MasterBootRecord[k+0x02];
		PartitionTable[i].firstCylinder = ((0xC0&MasterBootRecord[k+0x02]) << 2)&0x0300 + 0x00FF&MasterBootRecord[k+0x03];
		PartitionTable[i].type = 0xFF&MasterBootRecord[k+0x04];
		PartitionTable[i].lastHead = 0xFF&MasterBootRecord[k+0x05];
		PartitionTable[i].lastSector = 0x3F&MasterBootRecord[k+0x06];
		PartitionTable[i].lastCylinder = ((0xC0&MasterBootRecord[k+0x06]) << 2)&0x0300 + 0x00FF&MasterBootRecord[k+0x07];
		PartitionTable[i].LBA = 0x000000FF&MasterBootRecord[k+0x08] + ((0xFF&MasterBootRecord[k+0x09]) << 8)&0x0000FF00 + ((0xFF&MasterBootRecord[k+0x0A]) << 16)&0x00FF0000 + ((0xFF&MasterBootRecord[k+0x0B]) << 24)&0xFF000000; //Little endian
		PartitionTable[i].nbBlocks = 0x000000FF&MasterBootRecord[k+0x0C] + ((0xFF&MasterBootRecord[k+0x0D]) << 8)&0x0000FF00 + ((0xFF&MasterBootRecord[k+0x0E]) << 16)&0x00FF0000 + ((0xFF&MasterBootRecord[k+0x0F]) << 24)&0xFF000000; //Little endian
		k += 0x40; 
	}
	
}


void mbr_print_signature () {
	
	klog("MBR signature : Ox%x%x",0xFF&MasterBootRecord[0x1FF],0xFF&MasterBootRecord[0x1FE]);
	
}

void mbr_print_part_table () {
	
	int i ;
	
	for(i=1;i<2;i++)
	klog("\
	Partition Table :\n\
	Partition %d\n\
	Active: %d\n\
	Type: %d\n\
	start: %d %d %d Cylinder-Head-Sector\n\
	end  : %d %d %d Cylinder-Head-Sector\n\
	LBA: %d\n\
	Size: %d blocks",
	i+1,
	PartitionTable[i].active,
	PartitionTable[i].type,
	PartitionTable[i].firstCylinder,PartitionTable[i].firstHead,PartitionTable[i].firstSector,
	PartitionTable[i].lastCylinder,PartitionTable[i].lastHead,PartitionTable[i].lastSector,
	PartitionTable[i].LBA,PartitionTable[i].nbBlocks);
	
}
