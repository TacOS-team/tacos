/**
 * @file floppy_motor.c
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
 * Description de ce que fait le fichier
 */

#include <types.h>
#include <ioports.h>
#include "floppy_motor.h"
#include "floppy_utils.h"

static volatile floppy_motor_state motor_state[] = {0, 0, 0, 0};

// Amélioration possible: au lieu d'éteindre le moteur quand on lui 
// passe OFF, attendre un peu avec un thread parallele, au cas ou on 
// soit amené à le rallumer peu après. 
void floppy_motor(floppy_motor_state new_state)
{
	uint8_t drive = floppy_get_current_drive();
	uint8_t DOR = inb(FLOPPY_BASE + FLOPPY_DOR);
	
	if(new_state == ON)
	{
		// On ne met à jour que si il y a un réel changement
		if( motor_state[drive] == OFF) 
		{
			DOR |= (0x01 << (4+drive));
			outb(DOR, FLOPPY_BASE + FLOPPY_DOR);
			//TODO: Sleep (+- 500ms) 
		}
		motor_state[drive] = ON;
	}
	else
	{
		DOR &= ~(0x01 << (4+drive));
		outb(DOR, FLOPPY_BASE + FLOPPY_DOR);
		motor_state[drive] = OFF;
	}
}
