/**
 * @file interrupts.c
 */

#include <types.h>
#include <idt.h>
#include <i8259.h>
#include <interrupts.h>

/**
 * Le wrapper_array on le défini dans le interrupts_wrappers.S 
 * Le wrapper contient le code assembleur qui backup le contexte puis execute le handler.
 */
extern paddr_t interrupts_wrapper_array[INTERRUPT_NUM];

/* Tableau qui contient les handlers de toutes les interruptions. */
interrupt_handler_t interrupts_handler_array[INTERRUPT_NUM];

int interrupt_set_routine(uint8_t interrupt_id, interrupt_handler_t routine, uint8_t privilege)
{
	uint32_t flags;

	if (interrupt_id >= INTERRUPT_NUM) {
		return -1;
	}

	// arrêter les autres interruptions et exceptions et sauvegarde les flags
	asm volatile("pushfl ; popl %0":"=g"(flags)::"memory");
	asm("cli\n");

	interrupts_handler_array[interrupt_id] = routine;

	idt_set_handler(INTERRUPT_BASE + interrupt_id,
                  (paddr_t)interrupts_wrapper_array[interrupt_id], privilege);

	// XXX: Est-ce utile pour les interruptions logicielles ??
	i8259_enable_irq_line(interrupt_id);

	// Réactive les IRQs (restore flags) 
	asm volatile("push %0; popfl"::"g"(flags):"memory");

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

void make_trapgate_from_int(uint8_t interrupt_id)
{
	idt_set_handler_type(interrupt_id + INTERRUPT_BASE, IDT_TRAPGATE);
}

