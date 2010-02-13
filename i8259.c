/*
 * Configuration du i8259 pour gérer les interruptions.
 * @see http://www.etud.insa-toulouse.fr/~projet_tut_OS/w/Interruption
 */

#include "ioports.h"

/* Defines repris du projet coreboot (GPLv2) */
#define MASTER_PIC_ICW1         0x20
#define SLAVE_PIC_ICW1          0xa0
#define   ICW_SELECT            (1 << 4)
#define   OCW_SELECT            (0 << 4)
#define   ADI                   (1 << 2)
#define   SNGL                  (1 << 1)
#define   IC4                   (1 << 0)

#define MASTER_PIC_ICW2         0x21
#define SLAVE_PIC_ICW2          0xa1
#define   INT_VECTOR_MASTER     0x20
#define   IRQ0                  0x00
#define   IRQ1                  0x01
#define   INT_VECTOR_SLAVE      0x28
#define   IRQ8                  0x00
#define   IRQ9                  0x01

#define MASTER_PIC_ICW3         0x21
#define   CASCADED_PIC          (1 << 2)

#define MASTER_PIC_ICW4         0x21
#define SLAVE_PIC_ICW4          0xa1
#define   MICROPROCESSOR_MODE   (1 << 0)

#define SLAVE_PIC_ICW3          0xa1
#define    SLAVE_ID             0x02

#define MASTER_PIC_OCW1         0x21
#define SLAVE_PIC_OCW1          0xa1
#define    IRQ2                 (1 << 2)
#define    ALL_IRQS             0xff


void i8259_setup(void)
{
	/*
	 * Paramétrage du Intctl du master et du slave (ICW1).
	 * IC4 = besoin de ICW4
	 * @see datasheet d'intel !
	 */
	outb(ICW_SELECT | IC4, MASTER_PIC_ICW1);
	outb(ICW_SELECT | IC4, SLAVE_PIC_ICW1);

	/*
	 * Mapping des interruptions en configurant le ICW2. On va dire que pour 
	 * les IRQ il faut utiliser les interruptions processeur entre 32 et 47.
	 */
	outb(INT_VECTOR_MASTER | IRQ0, MASTER_PIC_ICW2);
	outb(INT_VECTOR_SLAVE  | IRQ8, SLAVE_PIC_ICW2);

	/*
	 * On va dire au Master qu'on a un Slave connecté.
	 * On masque là où il y a des slaves.
	 */
	outb(CASCADED_PIC, MASTER_PIC_ICW3);

	/*
	 * Et au Slave on lui indique où il est connecté.
	 */
	outb(SLAVE_ID, SLAVE_PIC_ICW3);

	/*
	 * On le passe en mode microprocesseur 8086/8088 non bufferisé
	 */
	outb(0x01, MASTER_PIC_ICW4);
	outb(0x01, SLAVE_PIC_ICW4);

	/*
	 * Maintenant que l'initialisation est faite, on peut régler les masques : 
	 */

	/*
	 * Sur le Slave on accepte tout.
	 */
	outb(ALL_IRQS, SLAVE_PIC_OCW1);

	/*
	 * Sur le maitre on accepte tout sauf l'IRQ2 car on a cascadé le 2 sur le slave.
	 */
	outb(ALL_IRQS & ~IRQ2, MASTER_PIC_OCW1);
}

