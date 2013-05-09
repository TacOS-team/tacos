/**
 * @file floppy.c
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
 * Floppy driver
 */

#include <fs/devfs.h>
#include <ioports.h>
#include <kstdio.h>
#include <klibc/string.h>
#include <types.h>
#include <klog.h>
#include "floppy_interrupt.h"
#include "floppy_utils.h"
#include "floppy_motor.h"
#include "floppy_dma.h"

#define FLOPPY_SECTOR_SIZE 512
#define FLOPPY_NB_HEADS 2
#define FLOPPY_SECTORS_PER_TRACK 18

static int floppy_open(open_file_descriptor *ofd);
static int floppy_close(open_file_descriptor *ofd);
static ssize_t floppy_read(open_file_descriptor *ofd, void* buf, size_t count, uint32_t offset);
static ssize_t floppy_write(open_file_descriptor *ofd, const void* buf, size_t count, uint32_t offset);

static blkdev_interfaces di = { 
	.read = floppy_read,
	.write = floppy_write,
	.ioctl = NULL,
	.open = floppy_open,
	.close = floppy_close
};

// Repositionne la tête de lecture sur le cylindre 0
int floppy_calibrate()
{
	int i, st0, cy1 = -1;
	int drive = floppy_get_current_drive();
	
	// Allumer le moteur
	floppy_motor(ON);
	
	// On essaye 5 fois (oui c'est totalement arbitraire)
	for(i=0; i<5; i++)
	{
		// Le recalibrage déclenche l'IRQ, on se prépare donc à attendre l'IRQ
		floppy_reset_irq();
		
		// Procedure de calibrage:
		// On envoi dans un premier temps la commande RECALIBRATE,
		// puis on envoi le numero du lecteur que l'on veut recalibrer
		floppy_write_command(RECALIBRATE);
		floppy_write_command(drive);
		
		// On attend la réponse
		floppy_wait_irq();
		
		// une fois l'IRQ arrivée, on peut récuperer les données de retour via SENSE_INTERRUPT
		floppy_sense_interrupt(&st0, &cy1);
		
		if(st0 & 0xC0)
		{
			static const char * status[] =
			{ 0, "error", "invalid", "drive" };
			klog("floppy_recalibrate: status = %s.", status[st0 >> 6]);
			klog("\tST0:0x%x.\nCY1:0x%x.", st0, cy1);
			continue;
		}
	
		if(!cy1) // si cy1=0, on a bien atteint le cylindre 0 et on peut arreter la calibration
		{
			floppy_motor(OFF);
			return 0;
		}
		
	}
	kerr("floppy_recalibrate: failure.");
	
	floppy_motor(OFF);
	
	return -1;
}

int init_floppy()
{
	int drive = floppy_get_current_drive();
	uint8_t drive_type = 0;
	uint8_t CCR;
	
	
	
	/* On vérifie qu'on a bien un controleur standard, sinon on affiche un warning */
	if(floppy_get_version() != 0x90)
		kerr("WARNING: Floppy driver may not work with 0x%x controler.", floppy_get_version());

	floppy_init_interrupt();

	floppy_reset_irq();
	
	// On fait le reset du controler + activation DMA/IRQ
	outb(0x00, FLOPPY_BASE + FLOPPY_DOR);
	outb(0x0C, FLOPPY_BASE + FLOPPY_DOR);
	
	floppy_wait_irq();
	floppy_sense_interrupt(NULL,NULL);
	
	// On regle la vitesse de transfert en fonction du type de disquette
	drive_type = floppy_get_type(drive);
	switch(drive_type)
	{
		case 1: // 300 kbps (01)
		case 3:
			CCR = 0x01;
			break;
			
		case 2: // 500 kbps (00)	
		case 4:
		
			CCR = 0x00;
			break;
		case 5: // 1 Mbps (11)
			CCR= 0x03;
			break;
			
		default:
			CCR = 0x00; // choix arbitraire btw
			break;
	}
	outb(CCR, FLOPPY_BASE + FLOPPY_CCR);
	
	
/* Totalement hardcodé et moche à fortiori*/
	floppy_write_command(SPECIFY);
    floppy_write_command(0xdf); /* steprate = 3ms, unload time = 240ms */
    floppy_write_command(0x02); /* load time = 16ms, no-DMA = 0 */
/* *************************************** */
	
	// On calibre le lecteur
	if(floppy_calibrate()) return -1;
	
	register_blkdev("fd0", &di);
	
	return 0;
}

int floppy_open(open_file_descriptor *ofd __attribute__((unused))) {
	/* TODO */
	return 0;
}

int floppy_close(open_file_descriptor *ofd __attribute__((unused))) {
	/* TODO */
	return 0;
}

static ssize_t floppy_read(open_file_descriptor *ofd __attribute__((unused)), void* buf, size_t count, uint32_t offset) {
	int offset_sector = offset / FLOPPY_SECTOR_SIZE;
	int offset_in_sector = offset % FLOPPY_SECTOR_SIZE;
	uint32_t cylinder;
	uint32_t head;
	uint32_t sector;
	size_t c;
	char buffer[FLOPPY_SECTOR_SIZE];
	size_t size = count;
	while (count) {
		
		cylinder = offset_sector / (FLOPPY_SECTORS_PER_TRACK * FLOPPY_NB_HEADS);
		head    = (offset_sector % (FLOPPY_SECTORS_PER_TRACK * FLOPPY_NB_HEADS)) / (FLOPPY_SECTORS_PER_TRACK);
		sector  = (offset_sector % (FLOPPY_SECTORS_PER_TRACK * FLOPPY_NB_HEADS)) % (FLOPPY_SECTORS_PER_TRACK);
		
		floppy_read_sector(cylinder, head, sector, buffer);
		c = FLOPPY_SECTOR_SIZE - offset_in_sector;
		if (count <= c) {
			memcpy(buf, buffer + offset_in_sector, count);
			count = 0;
		} else {
			memcpy(buf, buffer + offset_in_sector, c);
			count -= c;
			buf += c;
			offset_in_sector = 0;
			offset_sector++;
		}
	}
	return size;
}

static ssize_t floppy_write(open_file_descriptor *ofd __attribute__((unused)), const void* buf, size_t count, uint32_t offset) {
	int offset_sector = offset / FLOPPY_SECTOR_SIZE;
	int offset_in_sector = offset % FLOPPY_SECTOR_SIZE;
	uint32_t cylinder;
	uint32_t head;
	uint32_t sector;
	size_t c;
	size_t size = count;
	char buffer[FLOPPY_SECTOR_SIZE];

	while (count) {

		cylinder = offset_sector / (FLOPPY_SECTORS_PER_TRACK * FLOPPY_NB_HEADS);
		head    = (offset_sector % (FLOPPY_SECTORS_PER_TRACK * FLOPPY_NB_HEADS)) / (FLOPPY_SECTORS_PER_TRACK);
		sector  = (offset_sector % (FLOPPY_SECTORS_PER_TRACK * FLOPPY_NB_HEADS)) % (FLOPPY_SECTORS_PER_TRACK);

		floppy_read_sector(cylinder, head, sector, buffer);
		c = FLOPPY_SECTOR_SIZE - offset_in_sector;
		if (count <= c) {
			memcpy(buffer + offset_in_sector, buf, count);
			count = 0;
		} else {
			memcpy(buffer + offset_in_sector, buf, c);
			count -= c;
			buf += c;
			offset_in_sector = 0;
			offset_sector++;
		}
		floppy_write_sector(cylinder, head, sector, buffer);
	}
	return size;
}
