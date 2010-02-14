/* Dans ce fichier on va juste gérer les irq */

#include <types.h>
#include <idt.h>
#include <i8259.h>
#include "interrupts.h"

/* Nombre d'irq : */
#define INTERRUPT_NUM 16
/* On va définir une constante pour dire où on va mapper (pour l'idt) */
#define INTERRUPT_BASE 32 /* Juste après les exceptions */

/* Le wrapper_array on le défini dans le interrupts_wrappers.S */
/* Le wrapper contient le code assembleur qui backup le contexte puis execute le handler. */
extern paddr_t interrupt_wrapper_array[INTERRUPT_NUM];

/* Tableau qui contient les handlers de toutes les interruptions. */
interrupt_handler_t interrupts_handler_array[INTERRUPT_NUM];

int interrupt_set_routine(uint8_t interrupt_id, interrupt_handler_t routine)
{
	if (interrupt_id >= INTERRUPT_NUM) {
		return -1;
	}

	/* TODO : arrêter les autres interruptions et exceptions avec asm(cli), 
	 * et sauvegarder les flags ? */

	interrupts_handler_array[interrupt_id] = routine;

	idt_set_handler(INTERRUPT_BASE + interrupt_id, (paddr_t)routine, 0);

	i8259_enable_irq_line(interrupt_id);

	return 0;
}

int interrupt_disable(uint8_t interrupt_id)
{
	if (interrupt_id >= INTERRUPT_NUM) {
		return -1;
	}

	i8259_disable_irq_line(interrupt_id);

	interrupts_handler_array[interrupt_id] = (interrupt_handler_t)NULL;

	idt_disable_handler(INTERRUPT_BASE + interrupt_id);

	return 0;
}
