#include <types.h>
#include <stdio.h>
#include <ioports.h>
#include "floppy_motor.h"
#include "floppy_utils.h"
#include "floppy_dma.h"


#define floppy_dmalen 0x4800

// Déclaration du buffer qui servira aux transfert via l'ISA DMA
static const char floppy_dma_buffer[floppy_dmalen] __attribute__((aligned(0x8000)));

static uint8_t current_cylinder = -1;

void floppy_dma_init(floppy_io io_dir)
{
	//Pour initialiser le dma, on doit lui fournir l'adresse du buffer, ainsi que le nombre d'octets à lire
	union {
		uint8_t b[4]; // 4 x 1 octet
		uint32_t l;    // 1 x 4 octets
	} address, count;
	
	char dma_mode;
	
	count.l = (uint32_t) floppy_dmalen - 1; // "-1 car le DMA compte de address jusqu'a 0xffff en décrémentant, et non jusqu'à 0x0000
	address.l = (uint32_t) &floppy_dma_buffer;
	
	// Count doit tenir sur 16bits et Address sur 24bits.
	// De plus la somme des deux ne doit pas lever de retenue
	// On vérifie donc tout ça avant d'aller plus loin
	if((count.l >> 16)|(address.l >> 24)|(((address.l & 0xffff)+ count.l) >> 16))
	{
		printf("floppy_dma_init: Static buffer error.\n");
		return;
	}
	
	switch(io_dir)
	{
		case floppy_read: // READ = single/inc/no-auto/to-mem/chan2 = 01-0-0-01-10
			dma_mode = 0x46;
			break;
		case floppy_write: // WRITE = single/inc/no-auto/from-mem/chan2 = 01-0-0-10-10
			dma_mode = 0x4A;
			break;
		default:
			printf("floppy_dma_init: Invalid io_dir.\n");
			return;
	}
	
	outb(0x06, 0x0a);   // masque le channel 2

    outb(0xff, 0x0c);   // reset flip-flop
    outb(address.b[0], 0x04); //  - address low byte
    outb(address.b[1], 0x04); //  - address high byte

    outb(address.b[2], 0x81); // registre de page externe

    outb(0xff, 0x0c);   // reset flip-flop
    outb(count.b[0], 0x05); //  - count low byte
    outb(count.b[1], 0x05); //  - count high byte

    outb(dma_mode, 0x0b);   // donne le mode du DMA

    outb(0x02, 0x0a);   // démasque le channel 2
	
}

int floppy_cylinder(int base, int cylinder, floppy_io io_dir)
{
	
	
	return 0;
}

void floppy_read_sector(int base, int cylinder, int head, int sector, char* buffer)
{
	if(cylinder != current_cylinder)
	{
		// Si le cylindre que l'on a en mémoire n'est pas le bon, on charge le bon
		//floppy_read_cylinder(int base, int cylinder);
		current_cylinder = cylinder;
	}		
	
	// copier le secteur dans buffer
}	

void floppy_write_sector(int base, int cylinder, int head, int sector, char* buffer)
{
	if(cylinder != current_cylinder)
	{
		// Si le cylindre que l'on a en mémoire n'est pas le bon, on charge le bon
		//floppy_read_cylinder(int base, int cylinder);
		current_cylinder = cylinder;
	}
	
	// copier le buffer à l'offset de floppy_dma_buffer
	
	//floppy_write_cylinder(int base, int cylinder);
}
		
	
