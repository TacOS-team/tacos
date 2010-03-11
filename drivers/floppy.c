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

// Récupère les lecteurs floppy disponibles en interrogeant le CMOS
void floppy_detect_drives() {
	uint8_t drives;
	
	// Lecture du registre CMOS contenant les types de lecteur disquette
	outb(0x10, 0x70); // Selection du registre 0x10 du CMOS
	drives = inb(0x71);

	printf("Floppy drive 1: %s\n", drive_types[drives >> 4]);
	printf("Floppy drive 2: %s\n", drive_types[drives & 0xf]);
}

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
	
uint8_t floppy_get_version()
{
	floppy_write_command(FLOPPY_BASE, VERSION);
	
	return floppy_read_data(FLOPPY_BASE);
}
	
	
