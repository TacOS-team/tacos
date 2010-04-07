#include <ioports.h>
#include <stdio.h>
#include <types.h>
#include "floppy_utils.h"
#include "floppy_motor.h"

static volatile uint8_t current_drive = 0;

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

// Drive selection
uint8_t floppy_get_current_drive()
{
	return current_drive;
}

void floppy_set_current_drive(uint8_t drive)
{
	current_drive = drive;
}

int floppy_seek(int cylindre, int head)
{
	// Fonction similaire à floppy_calibrate, mais au lieu de chercher le cylindre 0 on cherche le cylindre cyl
	int i, drive, st0, cyl = -1;
	
	drive = floppy_get_current_drive();
	
	// Allumage du moteur
	floppy_motor(ON);
	
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
			kprintf("floppy_seek: status = %s.\n", status[st0 >> 6]);
			kprintf("ST0:0x%x.\nCYL:0x%x.\n", st0, cyl);
			continue;
		}
		
		if(cyl == cylindre) // si cyl=cylindre, on a bien atteint le cylindre voulu et on peut arreter le seek
		{
			floppy_motor(OFF);
			return 0;
		}
	}
	kprintf("floppy_seek: failure\n.");
	
	floppy_motor(OFF);
	
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
	uint8_t drives;
	
	// Lecture du registre CMOS contenant les types de lecteur disquette
	outb(0x10, 0x70); // Selection du registre 0x10 du CMOS
	drives = inb(0x71);

	kprintf("Floppy drive 0: %s\n", drive_types[floppy_get_type(0)]);
	kprintf("Floppy drive 1: %s\n", drive_types[floppy_get_type(1)]);
}


uint8_t floppy_get_version()
{
	floppy_write_command(VERSION);
	
	return floppy_read_data();
}
