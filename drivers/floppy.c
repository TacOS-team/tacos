#include <ioports.h>
#include <stdio.h>
#include <types.h>
#include "floppy_interrupt.h"

#define FLOPPY_BASE	0x03f0

enum floppy_registers
{
	FLOPPY_SRA = 0, // Status register A		-- read-only
	FLOPPY_SRB = 1, // Status register B		-- read-only
	FLOPPY_DOR = 2, // Digital output register
	FLOPPY_TDR = 3, // Tape drive register
	FLOPPY_MSR = 4, // Main status register		-- read-only
	FLOPPY_DSR = 4, // Datarate select register	-- write-only
	FLOPPY_FIFO = 5, // Data FIFO
	FLOPPY_DIR = 7, // Digital input register	-- read-only
	FLOPPY_CCR = 7  // Configuration control register -- write-only
};

enum floppy_commands
{
	READ_TRACK =			2,
	SPECIFY =				3,
	SENSE_DRIVE_STATUS =	4,
	WRITE_DATA = 			5,
	READ_DATA =				6,
	RECALIBRATE = 			7,
	SENSE_INTERRUPT =		8,
	READ_ID =				10,
	READ_DELETED_DATA =		12,
	FORMAT_TRACK =			13,
	SEEK =					15,
	VERSION =				16,
	SCAN_EQUAL =			17,
	PERPENDICULAR_MODE =	18,
	CONFIGURE =				19,
	LOCK = 					20,
	VERIFY =				22,
	SCAN_LOW_OR_EQUAL =		25,
	SCAN_HIGH_OR_EQUAL =	29
};

static const char * drive_types[8] = {
	"none",
	"360kB 5.25\"",
	"1,2MB 5.25\"",
	"720kB 5.25\"",
	"1.44MB 3.5\"",
	"2.88MB 3.5\"",
	"none",
	"none"
};

bool floppy_ready(int base)
{
	/* DEBUG 
	int i = inb(base + FLOPPY_MSR);
	printf("MSR:0x%x.\n", i&);
	*/
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

void floppy_sense_interrupt(int base, int* st0, int* cy1)
{
	floppy_write_command(base, SENSE_INTERRUPT);
	
	// Si on ne lance pas un sense en aveugle, on récupère les valeurs
	if(st0!=NULL && cy1!=NULL)
	{
		*st0 = floppy_read_data(base); // Status register 1
		*cy1 = floppy_read_data(base); // Cylindre actuel
	}
	else{
		floppy_read_data(base);
		floppy_read_data(base); 
	}
}

// Repositionne la tête de lecture sur le cylindre 0
int floppy_calibrate(int base, int drive)
{
	int i, st0, cy1 = -1;
	
	// TODO: Allumer le moteur
	
	// On essaye 5 fois (oui c'est totalement arbitraire
	for(i=0; i<5; i++)
	{
		// Le recalibrage déclenche l'IRQ, on se prépare donc à attendre l'IRQ
		floppy_reset_irq();
		
		// Procedure de calibrage:
		// On envoi dans un premier temps la commande RECALIBRATE,
		// puis on envoi le numero du lecteur que l'on veut recalibrer
		floppy_write_command(base, RECALIBRATE);
		floppy_write_command(base, drive);
		
		// On attend la réponse
		floppy_wait_irq();
		
		// une fois l'IRQ arrivée, on peut récuperer les données de retour via SENSE_INTERRUPT
		floppy_sense_interrupt(base, &st0, &cy1);
		
		if(st0 & 0xC0)
		{
			static const char * status[] =
			{ 0, "error", "invalid", "drive" };
			printf("floppy_recalibrate: status = %s.\n", status[st0 >> 6]);
			printf("ST0:0x%x.\nCY1:0x%x.\n", st0, cy1);
			continue;
		}
		
		if(!cy1) // si cy1=0, on a bien atteint le cylindre 0 et on peut arreter la calibration
		{
			// TODO: éteindre le moteur;
			return 0;
		}
	}
	printf("floppy_recalibrate: failure.\n");
	
	//TODO: éteindre le moteur
	
	return -1;
}

void init_floppy(int drive)
{
		int i;


	floppy_reset_irq();
	
	outb(0x00, FLOPPY_BASE + FLOPPY_DOR);
	outb(0x0C, FLOPPY_BASE + FLOPPY_DOR);
	
	floppy_wait_irq();

	floppy_sense_interrupt(FLOPPY_BASE, NULL,NULL);
	
	floppy_calibrate(FLOPPY_BASE, drive);
}
	
	

/*	
 * MISC  
 */

// Récupère les lecteurs floppy disponibles en interrogeant le CMOS
void floppy_detect_drives() {
	uint8_t drives;
	
	// Lecture du registre CMOS contenant les types de lecteur disquette
	outb(0x10, 0x70); // Selection du registre 0x10 du CMOS
	drives = inb(0x71);

	printf("Floppy drive 1: %s\n", drive_types[drives >> 4]);
	printf("Floppy drive 2: %s\n", drive_types[drives & 0xf]);
}

uint8_t floppy_get_version()
{
	floppy_write_command(FLOPPY_BASE, VERSION);
	
	return floppy_read_data(FLOPPY_BASE);
}
	
	
