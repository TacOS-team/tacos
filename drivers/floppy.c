#include <ioports.h>
#include <stdio.h>
#include <types.h>
#include "floppy_interrupt.h"
#include "floppy_utils.h"


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
	uint8_t drive_type = 0;
	uint8_t CCR;
	
	/* On vérifie qu'on a bien un controleur standard, sinon on affiche un warning */
	if(floppy_get_version() != 0x90)
		printf("WARNING: Floppy driver may not work with 0x%x controler.\n", floppy_get_version());

	floppy_reset_irq();
	
	// On fait le reset du controler + activation DMA/IRQ
	outb(0x00, FLOPPY_BASE + FLOPPY_DOR);
	outb(0x0C, FLOPPY_BASE + FLOPPY_DOR);
	
	floppy_wait_irq();
	floppy_sense_interrupt(FLOPPY_BASE, NULL,NULL);
	
	// On regle la vitesse de transfert en fonction du type de disquette
	drive_type = floppy_get_type(drive);
	switch(drive_type)
	{
		case 1: // 300 kbps (01)
		case 3:
			CCR = 0x01;
			break;
			
		case 2: // 500 kbps (00)	
		case 4:
		
			CCR = 0x00;
			break;
		case 5: // 1 Mbps (11)
			CCR= 0x03;
			break;
			
		default:
			CCR = 0x00; // choix arbitraire btw
			break;
	}
	outb(CCR, FLOPPY_BASE + FLOPPY_CCR);
	
	// On calibre le lecteur
	if(floppy_calibrate(FLOPPY_BASE, drive)) return -1;
}

	
	
