/**
 * @file ksyscall.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2015 TacOS developers.
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

#include <gdt.h>
#include <interrupts.h>
#include <ksyscall.h>
#include <klog.h>
#include <klibc/string.h>
#include <scheduler.h>
#include <ksignal.h>

/**
 * Table des syscall handlers.
 */
syscall_handler_t syscall_handler_table[MAX_SYSCALL_NB];

/**
 * @brief Point d'entrée des appels systèmes
 * C'est cette fonction qui est lancée à la réception de l'IRQ_SYSCALL
 * Son rôle est de récupérer l'identifiant de l'appel système, et
 * d'exécuter en conséquence le handler correspondant
 * 
 */
void syscall_entry()
{	
	uint32_t function, param1, param2, param3;
	syscall_handler_t handler;
	intframe* frame;
	
	/* Récupération des données empilées par l'interruption*/
	uint32_t ebp;
	asm volatile("mov %%ebp, %0" : "=r" (ebp));
	frame = (intframe*) (ebp + 8);
	function = frame->eax;
	param1 = frame->ebx;
	param2 = frame->ecx;
	param3 = frame->edx;

	//klog("syscall:0x%x 0x%x", frame+1, frame->kesp);

	/* Si on fait le syscall depuis l'user-mode, on sauvegarde l'esp user dans la tss */
	if(frame->cs == USER_CODE_SEGMENT) {
		get_default_tss()->esp1 = frame->esp;
	}
	
	/* On execute le handler correspondant, si il est bien présent dans la table */
	if(function < MAX_SYSCALL_NB && syscall_handler_table[function] != NULL)
	{
		handler = syscall_handler_table[function];
		handler(param1, param2, param3);
	}
	else {
		kerr("Unknown syscall handler (0x%x).\n", function);
		process_t *badboy = get_current_process();
		sys_kill(badboy->pid, SIGSYS, NULL);
	}
	
	
	asm("cli");
	get_default_tss()->esp0 = (uint32_t)(frame+1);
	asm("sti");
	
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

SYSCALL_HANDLER3(sys_dummy, uint32_t a,uint32_t b,uint32_t c) {
	float fa = 2.0f;
	float fb = 0.1f;
	int i;
	klog("sys - fucking - dummy %d %d %d", a,b,c);
	for(i=0; i<1000000;i++) {
		fa = fa*fb;
	}
}
