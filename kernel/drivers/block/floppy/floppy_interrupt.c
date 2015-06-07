/**
 * @file floppy_interrupt.c
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

#include <types.h>
#include <interrupts.h>
#include "floppy_utils.h"

volatile bool irq_received = false;

void floppy_handler(int interrupt_id __attribute__ ((unused)))
{
	irq_received = true;
}

void floppy_wait_irq()
{
	while(!irq_received);
	
	irq_received = false;
}

void floppy_init_interrupt()
{
	interrupt_set_routine(IRQ_FLOPPY, floppy_handler, 0);
}

void floppy_reset_irq()
{
	irq_received = false;
}

void floppy_sense_interrupt(int* st0, int* cy1)
{
	floppy_write_command(SENSE_INTERRUPT);
	
	// Si on ne lance pas un sense en aveugle, on récupère les valeurs
	if(st0!=NULL && cy1!=NULL)
	{
		*st0 = floppy_read_data(); // Status register 1
		*cy1 = floppy_read_data(); // Cylindre actuel
	}
	else{
		floppy_read_data();
		floppy_read_data(); 
	}
}


