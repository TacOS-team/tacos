/**
 * @file ksyscall.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010 - TacOS developers.
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


//Pour plus d'infos sur sysenter: Doc Intel 2B 4-483

#include <msr.h>
#include <stdio.h>
#include <string.h>
#include <interrupts.h>
#include "ksyscall.h"
#include <klog.h>

syscall_handler_t syscall_handler_table[MAX_SYSCALL_NB];

/**
 * @brief Point d'entrée des appels systèmes
 * C'est cet fonction qui est lancée à la réception de l'IRQ_SYSCALL
 * Sont rôle est de récupérer l'identifiant de l'appel système, et
 * d'exécuter en conséquence le handler correspondant
 * 
 * @param unused
 * 
 */
void syscall_entry(int interrupt_id __attribute__ ((unused)))
{	
	uint32_t function, param1, param2, param3;
	syscall_handler_t handler;
	
	/* On récupère les parametres */
	asm("":"=a"(function),"=b"(param1),"=c"(param2),"=d"(param3));

	/* On execute le handler correspondant, si il est bien présent dans la table */
	if(function < MAX_SYSCALL_NB && syscall_handler_table[function] != NULL)
	{
		handler = syscall_handler_table[function];
		handler(param1, param2, param3);
	}
	else
		kerr("Unknown syscall handler (0x%x).\n", function);
}

int syscall_set_handler(uint32_t syscall_id, syscall_handler_t handler)
{
	int ret = -1;
	
	/* Vérifie si l'identifiant de l'appel système n'est pas déja utilisé */
	if(syscall_id < MAX_SYSCALL_NB && syscall_handler_table[syscall_id] == NULL)
	{
		syscall_handler_table[syscall_id] = handler;
		ret = 0;
	}
	return ret;
}

void init_syscall()
{
	/* On associe le syscall entry à l'IRQ dédiée aux appels systèmes */
	interrupt_set_routine(IRQ_SYSCALL, syscall_entry, 3);
	
	/* Configuration de l'interruption en trap gate (pour la rendre interruptible) */
	make_trapgate_from_int(IRQ_SYSCALL);
	
	// Mise à zero de la table des handler
	memset(syscall_handler_table, 0, MAX_SYSCALL_NB*sizeof(syscall_handler_t));
}

