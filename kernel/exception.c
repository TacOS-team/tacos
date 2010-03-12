/* Dans ce fichier on va juste gérer les exceptions */

#include <types.h>
#include <idt.h>
#include "exception.h"

/* Nombre d'exceptions : */
#define EXCEPTION_NUM 32
/* On va définir une constante pour dire où on va mapper (pour l'idt) */
#define EXCEPTION_BASE 0

/* Le wrapper_array on le défini dans le exception_wrappers.S */
/* Le wrapper contient le code assembleur qui backup le contexte puis execute le handler. 
 * Certaines exceptions possèdent un code d'erreur, s'il n'y en a pas, on met 0. */
extern paddr_t exception_wrapper_array[EXCEPTION_NUM];

/* Tableau qui contient les handlers de toutes les exceptions. */
exception_handler_t exception_handler_array[EXCEPTION_NUM];

int exception_set_routine(uint8_t exception_id, exception_handler_t routine)
{ 
	uint32_t flags;
	if (exception_id >= EXCEPTION_NUM) {
		return -1;
	}

	// arrêter les autres interruptions et exceptions et sauvegarde les flags
	asm volatile("pushfl ; popl %0":"=g"(flags)::"memory");
	asm("cli\n");

	exception_handler_array[exception_id] = routine;

	idt_set_handler(EXCEPTION_BASE + exception_id, (paddr_t)exception_wrapper_array[exception_id], 0);

	// reenable irqs (restore flags) 
	asm volatile("push %0; popfl"::"g"(flags):"memory");

	return 0;
}

int exception_disable(uint8_t exception_id)
{
	if (exception_id >= EXCEPTION_NUM) {
		return -1;
	}

	exception_handler_array[exception_id] = (exception_handler_t)NULL;

	idt_disable_handler(EXCEPTION_BASE + exception_id);

	return 0;
}
