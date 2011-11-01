/**
 * @file scheduler.c
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


/**
 * @file scheduler.c
 */

#include <clock.h>
#include <context.h>
#include <debug.h>
#include <events.h>
#include <gdt.h>
#include <i8259.h>
#include <klog.h>
#include <kmalloc.h>
#include <kprocess.h>
#include <ksignal.h>
#include <interrupts.h>
#include <ioports.h>
#include <scheduler.h>
#include <types.h>
#include <vmm.h>
#include <pagination.h>

#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>

#define USER_PROCESS 0
#define KERNEL_PROCESS 1

static uint32_t quantum;	/* Quantum de temps alloué aux process */
static int sample_counter;	/* Compteur du nombre d'échantillonnage pour l'évaluation de l'usage CPU */

static process_t* idle_process = NULL;
static scheduler_descriptor_t* scheduler = NULL;

void idle()
{
	while(1)
		halt();
}

void set_scheduler(scheduler_descriptor_t* sched)
{
	scheduler = sched;
}


int is_schedulable(process_t* process) {
	if(	process == NULL ) {
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


/* Effectue le changement de contexte proprement dit */
void context_switch(int mode, process_t* current)
{
	uint32_t esp, eflags;
	uint16_t kss, ss, cs;
	
	/* Récupère l'adresse physique de répertoire de page du processus */
	paddr_t pd_paddr = vmm_get_page_paddr((vaddr_t) current->pd);
	
	/* Mise à jour des piles de la TSS */
	get_default_tss()->esp0	=	current->regs.kesp;
	get_default_tss()->ss0	=	current->regs.kss;
	
	ss = current->regs.ss;
	cs = current->regs.cs;
	
	eflags = (current->regs.eflags | 0x200) & 0xFFFFBFFF; // Flags permettant le changemement de contexte
	if(mode == USER_PROCESS)
	{
		kss = current->regs.kss;
		esp = current->regs.kesp;
	}
	else
	{
		kss = current->regs.ss;
		esp = current->regs.esp;
	}
	asm(
			"mov %0, %%ss;"
			"mov %1, %%esp;"
			"cmp %[KPROC], %[mode];" // if(mode != KERNEL_PROC)
			"je else;"
			"push %2;"
			"push %3;"
			"else:"
			"push %4;"
			"push %5;"
			"push %6;"
			::        
			"m" (kss), 		
			"m" (esp),
			"m" (ss), 					
			"m" (current->regs.esp), 	
			"m" (eflags), 				
			"m" (cs), 					
			"m" (current->regs.eip),
			[KPROC] "i"(KERNEL_PROCESS),
			[mode]	"g"(mode) 	
       );
       
	/* On push ensuite les registres de la tache à lancer */
	PUSH_CONTEXT(current);
	
	outb(0x20, 0x20);

	/* On lui remet sa page directory */
	pagination_load_page_directory((struct page_directory_entry *)pd_paddr);

	RESTORE_CONTEXT();
}

void* schedule(void* data __attribute__ ((unused)))
{
	uint32_t* stack_ptr;

	/* On met le contexte dans la structure "process"*/
	process_t* current = scheduler->get_current_process();
	
	/* On récupère le contexte du processus actuel uniquement si il a déja été lancé */
	if(current->state == PROCSTATE_RUNNING || current->state == PROCSTATE_WAITING || current->state == PROCSTATE_SUSPENDED)
	{	
		/* On récupere un pointeur de pile pour acceder aux registres empilés */
		asm("mov (%%ebp), %%eax; mov %%eax, %0" : "=m" (stack_ptr) : );
		
		intframe* frame = stack_ptr + 2;

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
		}
		else
		{
			current->regs.ss = frame->ss;
			current->regs.esp = frame->esp;
		}
		
		/* Sauver la TSS */
		/*current->regs.kesp = get_default_tss()->esp0;
		current->regs.kss  = get_default_tss()->ss0;*/
		
		current->user_time += quantum;
	}
	
	/* On recupere le prochain processus à executer.
	 * TODO: Dans l'idéal, on devrait ici faire appel à un scheduler, 
	 * qui aurait pour rôle de choisir le processus celon une politique spécifique */
	current = scheduler->get_next_process();
	
	if(!is_schedulable(current)) {
		/* Si on a rien à faire, on passe dans le processus idle */
		scheduler->inject_idle(idle_process);	
		current = idle_process;
	}
	else {
		/* Sinon on regarde si le process a des signaux en attente */
		exec_sighandler(current);
	}
	
	/* Evaluation de l'usage du CPU */
	current->current_sample++;
	sample_counter++;
	if(sample_counter >= CPU_USAGE_SAMPLE_RATE)
	{
		sample_CPU_usage();
		sample_counter = 0;
	}


	/* Si le processus courant n'a pas encore commencé son exécution, on le lance */
	if(current->state == PROCSTATE_IDLE)
	{
		current->state = PROCSTATE_RUNNING;
	}

	/* Mise en place de l'interruption sur le quantum de temps */
	/* XXX en lisant ça, je me demande si on pourrait pas le faire un peu plus tard, genre juste avant de changer de contexte */
	set_scheduler_event(schedule,NULL,quantum*1000);	

	/* Changer le contexte:*/
	if(current->regs.cs == KERNEL_CODE_SEGMENT)
		context_switch(KERNEL_PROCESS, current);
	else
		context_switch(USER_PROCESS, current);
		

	return NULL;
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
	
	idle_process = create_process(&idle_init);
	idle_process->state = PROCSTATE_IDLE;

}

void stop_scheduler()
{
	unset_scheduler_event();
}

void start_scheduler()
{
	set_scheduler_event(schedule,NULL,quantum);
}

int scheduler_add_process(process_t* proc)
{
	return scheduler->add_process(proc);
}

int scheduler_delete_process(int pid)
{
	return scheduler->delete_process(pid);
}

process_t* get_current_process()
{
	return scheduler->get_current_process();
}


void* sleep_callback( void* data )
{
	process_t* proc = (process_t*) data;
	proc->state = PROCSTATE_RUNNING;
	return NULL;
}

void halt()
{
	syscall(SYS_HLT, (uint32_t)NULL, (uint32_t)NULL,(uint32_t) NULL);
}
/*
void sys_hlt(uint32_t unused1 __attribute__ ((unused)), uint32_t unused2 __attribute__ ((unused)), uint32_t unused3 __attribute__ ((unused)))
*/
SYSCALL_HANDLER0(sys_hlt)
{
	asm("hlt");
}

SYSCALL_HANDLER1(sys_sleep, uint32_t delay)
{
	/* Désactivation de l'ordonnanceur */
	stop_scheduler();
	
	process_t* process = get_current_process();

	/* Passage du processus en waiting */
	process->state = PROCSTATE_WAITING;

	/* Adjout de l'évènement de fin de sleep */
	add_event(sleep_callback,(void*)process,delay);
	
	/* Scheduling immédiat */
	start_scheduler();
	
	
	while(process->state == PROCSTATE_WAITING);
	//dummy1();
		
	return;
}
