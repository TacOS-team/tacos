/**
 * @file floppy_dma.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012, 2013 - TacOS developers.
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
 * Fonctions liées à l'utilisation du DMA dans le driver disquette
 */

#include <types.h>
#include <klibc/string.h>
#include <ioports.h>
#include <klog.h>
#include "floppy_motor.h"
#include "floppy_utils.h"
#include "floppy_interrupt.h"
#include "floppy_dma.h"


#define floppy_dmalen 0x4800

#define floppy_head2_start floppy_dmalen/2

// Déclaration du buffer qui servira aux transfert via l'ISA DMA
static const char floppy_dma_buffer[floppy_dmalen] __attribute__((aligned(0x8000)));

static uint8_t current_cylinder = -1;

void floppy_dma_init(floppy_io io_dir)
{
	//Pour initialiser le dma, on doit lui fournir l'adresse du buffer, ainsi que le nombre d'octets à lire
	union {
		uint8_t b[4]; // 4 x 1 octet
		uint32_t l;    // 1 x 4 octets
	} address, count;
	
	char dma_mode;
	
	count.l = (uint32_t) floppy_dmalen - 1; // "-1 car le DMA compte de address jusqu'a 0xffff en décrémentant, et non jusqu'à 0x0000
	address.l = (uint32_t) &floppy_dma_buffer;
	
	// Count doit tenir sur 16bits et Address sur 24bits.
	// De plus la somme des deux ne doit pas lever de retenue
	// On vérifie donc tout ça avant d'aller plus loin
	if((count.l >> 16)|(address.l >> 24)|(((address.l & 0xffff)+ count.l) >> 16))
	{
		kerr("Static buffer error.\n");
		return;
	}
	
	switch(io_dir)
	{
		case FLOPPY_READ: // READ = single/inc/no-auto/to-mem/chan2 = 01-0-0-01-10
			dma_mode = 0x46;
			break;
		case FLOPPY_WRITE: // WRITE = single/inc/no-auto/from-mem/chan2 = 01-0-0-10-10
			dma_mode = 0x4A;
			break;
		default:
			kerr("Invalid io_dir (0x%x).", io_dir);
			return;
	}
	
	outb(0x06, 0x0a);   // masque le channel 2

    outb(0xff, 0x0c);   // reset flip-flop
    outb(address.b[0], 0x04); //  - address low byte
    outb(address.b[1], 0x04); //  - address high byte

    outb(address.b[2], 0x81); // registre de page externe

    outb(0xff, 0x0c);   // reset flip-flop
    outb(count.b[0], 0x05); //  - count low byte
    outb(count.b[1], 0x05); //  - count high byte

    outb(dma_mode, 0x0b);   // donne le mode du DMA

    outb(0x02, 0x0a);   // démasque le channel 2
	
}

static int floppy_cylinder(int cylinder, floppy_io io_dir)
{
	uint8_t command = 0;
	uint8_t drive = floppy_get_current_drive();
	int i; 
	const uint8_t flags = 0xC0; // Multitrack et MFM activés
	
	// Variables recevant les valeurs de retour de la lecture
	uint8_t st0, st1, st2, rcy, rhe, rse, bps;
	int error = 0;
	
	switch(io_dir)
	{
		case FLOPPY_WRITE:
			command = WRITE_DATA | flags;
			break;
		case FLOPPY_READ:
			command = READ_DATA | flags;
			break;
		default:
			kerr("Invalid io_dir (0x%x).", io_dir);
			return -1;

	}
	
	// On lit avec les deux têtes, on les met donc toutes en position
	if(floppy_seek(cylinder, 0)) return -1;
	if(floppy_seek(cylinder, 1)) return -1;
	
	// On s'accord 5 essais, totalement arbitraire, à calibrer donc.
	for(i=0; i<5; i++)
	{
		// Allumage moteur
		floppy_motor(ON);
		
		// Initialisation DMA
		floppy_dma_init(io_dir);
		
		//TODO: Sleep pour attendre que le seek soit bien fini
		
		
		// Envoi de la commande
		//1) Commande
		//2) (head<<2)|drive
		//3) Numero du cylindre
		//4) head
		//5) Numero du premier secteur
		//6) Taille des secteur(2=512 bytes, ce qui est le cas pour toutes les floppy...)
		//7) Nombre de secteurs à lire
		//8) 0x1b (taille par défaut de GAP1)
		//9) 0xff (??)
		floppy_write_command(command);
		floppy_write_command(drive&0x03); // head = 0 dans le cas du MT, et drive&0x03 au cas ou drive > 3
		floppy_write_command(cylinder);
		floppy_write_command(0); // voir plus haut
		floppy_write_command(1); // On compte les secteurs a partir de 1
		floppy_write_command(2);
		floppy_write_command(18); // 18 secteurs par piste
		floppy_write_command(0x1b);
		floppy_write_command(0xff);
		
		floppy_wait_irq(); 
		
		// On verifie que la lecture c'est correctement déroulée
		
		// Informations de statut
		st0 = floppy_read_data();
        st1 = floppy_read_data();
        st2 = floppy_read_data();
        
        // Informations sur les CHS
        rcy = floppy_read_data(); // Cylindre
        rhe = floppy_read_data(); // Head
        rse = floppy_read_data(); // Secteur
        bps = floppy_read_data(); // Nomber de byte par secteur
        
        /* Traitement des erreurs repompée */
        if(st0 & 0xC0) {
            static const char * status[] =
            { 0, "error", "invalid command", "drive not ready" };
            klog("floppy_do_sector: status = %s", status[st0 >> 6]);
            error = 1;
        }
        if(st1 & 0x80) {
            kerr("end of cylinder");
            error = 1;
        }
        if(st0 & 0x08) {
            kerr("drive not ready ");
            error = 1;
        }
        if(st1 & 0x20) {
            kerr("CRC error");
            error = 1;
        }
        if(st1 & 0x10) {
            kerr("controller timeout");
            error = 1;
        }
        if(st1 & 0x04) {
            kerr("no data found\n");
            error = 1;
        }
        if((st1|st2) & 0x01) {
            kerr("no address mark found");
            error = 1;
        }
        if(st2 & 0x40) {
            kerr("deleted address mark");
            error = 1;
        }
        if(st2 & 0x20) {
            kerr("CRC error in data");
            error = 1;
        }
        if(st2 & 0x10) {
            kerr("wrong cylinder");
            error = 1;
        }
        if(st2 & 0x04) {
            kerr("uPD765 sector not found");
            error = 1;
        }
        if(st2 & 0x02) {
            kerr("bad cylinder");
            error = 1;
        }
        if(bps != 0x2) {
            kerr("wanted 512B/sector, got %d", (1<<(bps+7)));
            error = 1;
        }
        if(st1 & 0x02) {
            kerr("not writable");
            error = 2;
        }

        if(!error) {
            floppy_motor(OFF);
            return 0;
        }
        if(error > 1) {
            kerr("not retrying...(rcy = %0x%x, rhe = %0x%x, rse = %0x%x)", rcy, rhe, rse);
            floppy_motor(OFF); 
            return -2;
        }
	
	}
	
	return 0;
}

void floppy_read_sector(int cylinder, int head, int sector, char* buffer)
{
	if(cylinder != current_cylinder)
	{
		// Si le cylindre que l'on a en mémoire n'est pas le bon, on charge le bon
		floppy_cylinder(cylinder, FLOPPY_READ);
		current_cylinder = cylinder;
	}		
	
	// copier le secteur dans buffer
	memcpy(buffer, floppy_dma_buffer+(head*floppy_head2_start)+512*sector, 512);
}	

void floppy_write_sector(int cylinder, int head, int sector, char* buffer)
{
	if(cylinder != current_cylinder)
	{
		// Si le cylindre que l'on a en mémoire n'est pas le bon, on charge le bon
		floppy_cylinder(cylinder, FLOPPY_READ);
		current_cylinder = cylinder;
	}
	
	// copier le buffer à l'offset de floppy_dma_buffer
	memcpy((void*)floppy_dma_buffer+(head*floppy_head2_start)+512*sector, buffer,  512);
	
	// Ecrit la nouvelle version du cylindre
	floppy_cylinder(cylinder, FLOPPY_WRITE);
}
		
	
