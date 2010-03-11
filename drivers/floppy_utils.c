#include <ioports.h>
#include <stdio.h>
#include <types.h>
#include "floppy_utils.h"

static volatile uint8_t current_drive = 0;

bool floppy_ready(int base)
{
	return ((0x80 & inb(base + FLOPPY_MSR))!=0);
}

int floppy_write_command(int base, char cmd)
{
	int ret = -1;
	
	/*TODO: Ajouter un timeout plutot que d'abandonner si la FIFO n'est pas prête */
	
	if(floppy_ready(base)) // On envoi la commande que si la FIFO est prête
	{
		outb(cmd, base + FLOPPY_FIFO);
		ret = 0;
	}
	
	return ret;
}

uint8_t floppy_read_data(int base)
{
	uint8_t ret = 0;
	
	if(floppy_ready(base)) // On ne lis que si la FIFO est prête
	{
		ret = inb(base + FLOPPY_FIFO);
	}
	
	return ret;
}

// Drive selection
uint8_t floppy_get_current_drive()
{
	return current_drive;
}

void floppy_set_current_drive(uint8_t drive)
{
	current_drive = drive;
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
	uint8_t drives;
	
	// Lecture du registre CMOS contenant les types de lecteur disquette
	outb(0x10, 0x70); // Selection du registre 0x10 du CMOS
	drives = inb(0x71);

	printf("Floppy drive 0: %s\n", drive_types[floppy_get_type(0)]);
	printf("Floppy drive 1: %s\n", drive_types[floppy_get_type(1)]);
}


uint8_t floppy_get_version()
{
	floppy_write_command(FLOPPY_BASE, VERSION);
	
	return floppy_read_data(FLOPPY_BASE);
}
