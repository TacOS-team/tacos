#include <types.h>
#include <stdio.h>
#include <ioports.h>
#include "floppy_motor.h"
#include "floppy_utils.h"

static volatile floppy_motor_state motor_state[] = {0, 0, 0, 0};

// Amélioration possible: au lieu d'éteindre le moteur quand on lui 
// passe OFF, attendre un peu avec un thread parallele, au cas ou on 
// soit amené à le rallumer peu après. 
void floppy_motor(int base, floppy_motor_state new_state)
{
	uint8_t drive = floppy_get_current_drive();
	uint8_t DOR = inb(base + FLOPPY_DOR);
	
	if(new_state == ON)
	{
		// On ne met à jour que si il y a un réel changement
		if( motor_state[drive] == OFF) 
		{
			DOR |= (0x01 << (4+drive));
			outb(DOR, base + FLOPPY_DOR);
			//TODO: Sleep (+- 500ms) 
		}
		motor_state[drive] = ON;
	}
	else
	{
		DOR &= ~(0x01 << (4+drive));
		outb(DOR, base + FLOPPY_DOR);
		motor_state[drive] = OFF;
	}
}
