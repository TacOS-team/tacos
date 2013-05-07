/**
 * @file exception.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012, 2013 - TacOS developers.
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
 * @brief Gestion des exceptions.
 */

#include <idt.h>
#include <exception.h>

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
