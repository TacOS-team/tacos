#include <types.h>
#include <interrupts.h>
#include <stdio.h>
#include "floppy_utils.h"

volatile bool irq_received = FALSE;

void floppy_handler(int interrupt_id)
{
	irq_received = TRUE;
}

void floppy_wait_irq()
{
	while(!irq_received);
	
	irq_received = FALSE;
}

void floppy_init_interrupt()
{
	interrupt_set_routine(IRQ_FLOPPY, floppy_handler);
}

void floppy_reset_irq()
{
	irq_received = FALSE;
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


