/**
 * @file scheduler.c
 *
 * @author TacOS developers 
 *
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


#include <clock.h>
#include <context.h>
#include <cpu.h>
#include <debug.h>
#include <events.h>
#include <gdt.h>
#include <i8259.h>
#include <interrupts.h>
#include <ioports.h>
#include <klibc/string.h>
#include <klog.h>
#include <kmalloc.h>
#include <kprocess.h>
#include <ksignal.h>
#include <pagination.h>
#include <scheduler.h>
#include <types.h>
#include <vmm.h>

#define USER_PROCESS 0
#define KERNEL_PROCESS 1

static uint32_t quantum;	/* Quantum de temps alloué aux process */
static int resched;
static int sched_planned;

static process_t* idle_process = NULL;
static scheduler_descriptor_t* scheduler = NULL;
static int scheduler_activated;

static process_t* current_process = NULL;

void idle()
{
	while(1)
		asm("hlt");
}

void set_scheduler(scheduler_descriptor_t* sched)
{
	scheduler = sched;
}


int is_schedulable(process_t* process) {
	if (process == NULL) {
		return 0;
	}
	switch (process->state) {
		case PROCSTATE_TERMINATED:
			return 0;
		case PROCSTATE_SUSPENDED:
		case PROCSTATE_WAITING:
			return signal_pending(process) != 0;
		default:
			return 1;
	}
}

void update_stats() {
	if (current_process && current_process != idle_process) {
		if (current_process->regs.cs == KERNEL_CODE_SEGMENT) {
			current_process->sys_time++;
			sys_time++;
		} else {
			current_process->user_time++;
			user_time++;
		}
	} else {
		idle_time++;
	}
}

/* Effectue le changement de contexte proprement dit */
static void context_switch(int mode, process_t* current)
{
	uint32_t eflags;
	uint32_t kesp;
	uint16_t kss;
	
	current_process = current;

	/* Récupère l'adresse physique de répertoire de page du processus */
	paddr_t pd_paddr = vmm_get_page_paddr((vaddr_t) current->pd);
	
	/* Mise à jour des piles de la TSS */
	get_default_tss()->esp0	=	current->regs.kesp;
	get_default_tss()->ss0	=	current->regs.kss;

	eflags = (current->regs.eflags | FLAGS_IF) & ~(FLAGS_NT); // Flags permettant le changemement de contexte (Réactivation des interruption, et désactivation de nested task)
	
	if(mode == USER_PROCESS) /* Si on est en user space, l'esp kernel sera celui de la tss */
	{
		kss = current->regs.kss;
		kesp = current->regs.kesp;
	}
	else /* En kernel space, on revien à l'esp récupéré par le scheduler */
	{
		kss = current->regs.ss;
		kesp = current->regs.esp;
	}
	
	/* On s'assure que SS est bien celui du kernel */
	asm("mov %0, %%ss;"::"m" (kss));
	
	/* On remet esp à son origine */
	asm("mov %0, %%esp;"::"m" (kesp));
	
	/* On empile dans tous les cas eflags, cs et eip.
	 * Dans le cas où on retourne en userspace, on empile ss et esp avant
	 */
	if(mode != KERNEL_PROCESS) {
		asm(
			"push %0;"
			"push %1;"::	        
			"m" (current->regs.ss), 					
			"m" (current->regs.esp)
		);
	}
	
	asm(		
			"push %0;"
			"push %1;"
			"push %2;"
			::        
			"m" (eflags), 				
			"m" (current->regs.cs), 					
			"m" (current->regs.eip)
	);
       
	/* On push ensuite les registres de la tache à lancer */
	PUSH_CONTEXT(current);
	
	outb(0x20, 0x20);
	
	/* On lui remet sa page directory */
	pagination_load_page_directory((struct page_directory_entry *)pd_paddr);

	scheduler_activated = 1;
	/* Pop les registres généraux puis iret */
	RESTORE_CONTEXT();
}

static void copy_context_current(process_t* current, intframe* frame) {

	current->regs.eflags = frame->eflags;
	current->regs.cs  = frame->cs;
	current->regs.eip = frame->eip;
	current->regs.eax = frame->eax;
	current->regs.ecx = frame->ecx;
	current->regs.edx = frame->edx;
	current->regs.ebx = frame->ebx;
	current->regs.ebp = frame->ebp;
	current->regs.esi = frame->esi;
	current->regs.edi = frame->edi;
	current->regs.fs = frame->fs;
	current->regs.gs = frame->gs;
	current->regs.ds = frame->ds;
	current->regs.es = frame->es;

	/* Si on ordonnance une tache en cours d'appel systeme.. */
	if(current->regs.cs == KERNEL_CODE_SEGMENT)
	{
		current->regs.ss = get_default_tss()->ss0;
		current->regs.esp = frame->kesp + 12;	/* Valeur hardcodée, je cherche encore un moyen d'éviter ça... */
		current->regs.kesp = current->regs.esp;
	}
	else
	{
		current->regs.ss = frame->ss;
		current->regs.esp = frame->esp;
		current->regs.kesp = (uint32_t)(frame+1);
	}
/*
		current->regs.kss = get_default_tss()->ss0;
		current->regs.kesp = get_default_tss()->esp0;
	*/	
}




void do_schedule()
{
	sched_planned = 0;
	if (!resched || !scheduler_activated) {
		return;
	}
	resched = 0;
	scheduler_activated = 0;
	
	int exec_signal = 0;

	/* On récupere un pointeur de pile pour acceder aux registres empilés
	 * C'est fait au début pour éviter de toucher à ebp.
	 * */
	intframe* stack_ptr;
	GET_INTFRAME(stack_ptr);

	/* On met le contexte dans la structure "process"*/
	process_t* current = get_current_process();
	if (!current) current = idle_process;
	
	/* On recupere le prochain processus à executer. */
	process_t *next = scheduler->get_next_process();
	
	if(is_schedulable(next)) {
		if (next->sig_interruptable) {
			/* On regarde si le process a des signaux en attente */
			if (signal_pending(next) && current == next) {
				// backup de current pour éviter de mettre dans sigframe de vieilles données.
				copy_context_current(current, stack_ptr);
			}
			exec_signal = exec_sighandler(next);
		} else if (next->state == PROCSTATE_WAITING || next->state == PROCSTATE_SUSPENDED) {
			next->state = PROCSTATE_RUNNING;
		}
	} else {
		/* Sinon, si on a rien à faire, on passe dans le processus idle */
		next = idle_process;
	}

	/* Si la décision de l'ordo est de ne pas changer le processus courant */
	if (current == next && next->state != PROCSTATE_IDLE && !exec_signal) {
		scheduler_activated = 1;
		return;
	}

	/* On fait une copie du contexte du processus actuel uniquement si il a déja été lancé */
	if (current != idle_process && current != next && current->state != PROCSTATE_IDLE) {
		copy_context_current(current, stack_ptr);
	}

	/* Si le processus courant n'a pas encore commencé son exécution, on le lance */
	if (next->state == PROCSTATE_IDLE) {
		next->state = PROCSTATE_RUNNING;
	} 

	/* Changer le contexte:*/
	if (next->regs.cs == KERNEL_CODE_SEGMENT)
		context_switch(KERNEL_PROCESS, next);
	else
		context_switch(USER_PROCESS, next);
}

void* schedule(void* data __attribute__ ((unused))) {
	resched = 1;

	// Ce test sert à éviter une accumulation d'évènements schedule.
	if (sched_planned == 0) {
		/* Mise en place de l'interruption sur le quantum de temps */
		add_event(schedule, NULL, quantum * 1000);
		sched_planned = 1;
	}

	return NULL;
}

void force_reschedule() {
	resched = 1;
}


void init_scheduler(int Q)
{
	quantum = Q;
	
	process_init_data_t idle_init;
	
	idle_init.name = "idle";
	idle_init.args = "idle";
	idle_init.exec_type = EXEC_KERNEL;
	idle_init.data = (void*)idle;
	idle_init.mem_size = 0;
	idle_init.entry_point = 0;
	idle_init.stack_size = 0x1000;
	idle_init.priority = 0;
	idle_init.ppid = 0;
	idle_init.file = NULL;
	
	idle_process = create_process(&idle_init, 1);
	idle_process->state = PROCSTATE_IDLE;

	resched = 1;
}

void stop_scheduler()
{
	scheduler_activated = 0;
}

void start_scheduler()
{
	scheduler_activated = 1;
	add_event(schedule, NULL, quantum * 1000);
}

int scheduler_add_process(process_t* proc)
{
	return scheduler->add_process(proc);
}

int scheduler_delete_process(int pid)
{
	if (current_process && current_process->pid == pid) current_process = NULL;
	return scheduler->delete_process(pid);
}

process_t* get_current_process()
{
	return current_process;
}


static void* sleep_callback( void* data )
{
	process_t* proc = (process_t*) data;
	proc->state = PROCSTATE_RUNNING;
	return NULL;
}


SYSCALL_HANDLER1(sys_sleep, uint32_t delay)
{
	process_t* process = get_current_process();

	/* Adjout de l'évènement de fin de sleep */
	add_event(sleep_callback,(void*)process,delay);

	set_state_waiting();
}

void set_state_waiting() {
	process_t* process = get_current_process();
	/* Passage du processus en waiting */
	process->state = PROCSTATE_WAITING;
	force_immediate_resched();
	while(process->state == PROCSTATE_WAITING) asm("hlt"); // Ne peut normalement plus rentrer dans cette boucle !
}

void force_immediate_resched() {
	resched = 1;
	// Fait ce que le CPU fait lors d'une interruption :
	asm(
			"pushfl\n\t"
			"push %cs\n\t"
			"push $fin_force_immediate_resched\n\t"
		);
	// Fait ce que le wrapper d'interruption aurait fait :
	asm(
			"pusha \n\t"
			"push %gs \n\t"
			"push %fs \n\t"
			"push %ds \n\t"
			"push %es \n\t"

			"push $0 \n\t"
			"call do_schedule \n"
			"add $64, %esp \n\t"
			"fin_force_immediate_resched:"
	   );
}
