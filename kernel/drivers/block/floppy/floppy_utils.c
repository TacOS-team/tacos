/**
 * @file floppy_utils.c
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

#include <ioports.h>
#include <types.h>
#include <klog.h>
#include "floppy_interrupt.h"
#include "floppy_utils.h"
#include "floppy_motor.h"

static const char * drive_types[8] __attribute__((unused))  = {
	"none",
	"360kB 5.25\"",
	"1,2MB 5.25\"",
	"720kB 5.25\"",
	"1.44MB 3.5\"",
	"2.88MB 3.5\"",
	"none",
	"none"
};

bool floppy_ready()
{
	return ((0x80 & inb(FLOPPY_BASE + FLOPPY_MSR))!=0);
}

int floppy_write_command(char cmd)
{
	int ret = -1;
	
	/*TODO: Ajouter un timeout plutot que d'abandonner si la FIFO n'est pas prête */
	
	if(floppy_ready()) // On envoi la commande que si la FIFO est prête
	{
		outb(cmd, FLOPPY_BASE + FLOPPY_FIFO);
		ret = 0;
	}
	
	return ret;
}

uint8_t floppy_read_data()
{
	uint8_t ret = 0;
	
	if(floppy_ready()) // On ne lis que si la FIFO est prête
	{
		ret = inb(FLOPPY_BASE + FLOPPY_FIFO);
	}
	
	return ret;
}


int floppy_seek(int drive, int cylindre, int head)
{
	// Fonction similaire à floppy_calibrate, mais au lieu de chercher le cylindre 0 on cherche le cylindre cyl
	int i, st0, cyl = -1;
	

	// Allumage du moteur
	floppy_motor(drive, ON);
	
	// On fait 5 tentative à titre arbitraire
	for(i=0; i<5; i++)
	{
		// Protocole de la commande SEEK:
		// Premier parametre: (head <<2)|drive
		// Deuxieme parametre: cylindre
		// Attente IRQ
		floppy_write_command(SEEK);
		floppy_write_command((head<<2)|drive);
		floppy_write_command(cylindre);

		//Attente de l'IRQ
		floppy_wait_irq();

		//Récuperation des status
		floppy_sense_interrupt(&st0, &cyl);
		
		if(st0 & 0xC0)
		{
			static const char * status[] =
			{ 0, "error", "invalid", "drive" };
			kerr("status = %s.", status[st0 >> 6]);
			kerr("\tST0:0x%x.\nCYL:0x%x.\n", st0, cyl);
			continue;
		}
		
		if(cyl == cylindre) // si cyl=cylindre, on a bien atteint le cylindre voulu et on peut arreter le seek
		{
			floppy_motor(drive, OFF);
			return 0;
		}
	}
	kerr("failure.");
	
	floppy_motor(drive, OFF);
	
	return -1;
}

/*	
 * MISC  
 */

uint8_t floppy_get_type(int drive)
{
	uint8_t drive_type;
	
	outb(0x10, 0x70);
	drive_type = inb(0x71);
	
	drive_type = (drive_type>>(4*(1-drive)))&0xf;
	
	return drive_type;
}

// Récupère les lecteurs floppy disponibles en interrogeant le CMOS
void floppy_detect_drives() {
	kdebug("Floppy drive 0: %s", drive_types[floppy_get_type(0)]);
	kdebug("Floppy drive 1: %s", drive_types[floppy_get_type(1)]);
}


uint8_t floppy_get_version()
{
	floppy_write_command(VERSION);
	
	return floppy_read_data();
}
