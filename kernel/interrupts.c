/**
 * @file interrupts.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012 - TacOS developers.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * @section DESCRIPTION
 *
 * Description de ce que fait le fichier
 */

#include <types.h>
#include <idt.h>
#include <i8259.h>
#include <interrupts.h>

/**
 * Le wrapper_array on le défini dans le interrupts_wrappers.S 
 * Le wrapper contient le code assembleur qui backup le contexte puis execute 
 * le handler.
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

