/**
 * @file ksignal.c
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

/* Kernel */
#include <gdt.h>
#include <interrupts.h>
#include <klog.h>
#include <kprocess.h>
#include <ksignal.h>
#include <kstdio.h>
#include <pagination.h>
#include <scheduler.h>
#include <syscall_values.h>
#include <vmm.h>

/**
 * Nom des signaux. Utile pour afficher leurs noms à la place de leurs numéros.
 */
const char* signal_names[] = {"???", "SIGHUP", "SIGINT", "SIGQUIT", "SIGILL", "SIGTRAP",
	"SIGABRT", "SIGBUS ", "SIGFPE ", "SIGKILL", "SIGUSR1", "SIGSEGV", "SIGUSR2",
	"SIGPIPE", "SIGALRM", "SIGTERM", "SIGSTKFLT", "SIGCHLD", "SIGCONT", "SIGSTOP",
	"SIGTSTP", "SIGTTIN", "SIGTTOU", "SIGURG", "SIGSYS", "SIGRTMIN"};

typedef struct {
	vaddr_t ret_addr;
	int sig;
	regs_t context;
	uint32_t state;
	sigset_t mask;
	char retcode[8];
} sigframe;

// Dupliqué de la libc.
static int sigaddset(sigset_t *set, int signum)
{
	int ret = -1;
	if(set != NULL)
	{
		ret = 0;
		*set |= (1<<signum);
	}
	return ret;
}

static int sigdelset(sigset_t *set, int signum)
{
	int ret = -1;
	if(set != NULL)
	{
		ret = 0;
		*set &= ~(1<<signum);
	}
	return ret;
}






/*
 * TODO: gérer SIG_IGN et SIG_DFL 
 */
SYSCALL_HANDLER3(sys_signal, uint32_t signum, sighandler_t handler, sighandler_t* ret)
{
	process_t* current = get_current_process();

	if(signum <= NSIG)
	{
		current->signal_data.handlers[signum] = handler;
		*ret = handler;
	}
}

SYSCALL_HANDLER3(sys_sigprocmask, uint32_t how, sigset_t* set, sigset_t* oldset)
{
	process_t* current = get_current_process();
	
	/* On récupère l'ancien set */
	if(oldset != NULL)
		*oldset = current->signal_data.mask;
	
	/* Et on met à jour le nouveau */
	switch(how)
	{
		case SIG_SETMASK:
			current->signal_data.mask = *set;
			break;
		case SIG_UNBLOCK:
			current->signal_data.mask &= ~(*set);
			break;
		case SIG_BLOCK:
			current->signal_data.mask |= (*set);
			break;
		default:
			kerr("Invalid sigprocmask command (0x%x).", how);
	}
} 

/* TODO: d'après POSIX, kill(-1, sig) doit envoyer le signal à tous les processus possibles... */
SYSCALL_HANDLER3(sys_kill, int pid, int signum, int* ret)
{	
	process_t* process = find_process(pid);
	
	int retour = -1;
	
	if(process != NULL)
	{
		retour = sigaddset( &(process->signal_data.pending_set), signum );
		
		klog("%d sending signal %s(%d) to pid %d.", get_current_process()->pid, signal_names[signum], signum, pid);
	}
	else
	{
		kerr("Process not found (%d).", pid);
	}
	
	if(ret!=NULL)
		*ret = retour;
}


SYSCALL_HANDLER0(sys_sigret)
{
	/* 
	 * Je met un petit trophée ici, ce code a marche du premier coup
	 */
	uint32_t* stack_ptr;
	/* On récupere un pointeur de pile pour acceder aux registres empilés */
	GET_INTFRAME(stack_ptr);

	sigframe* sframe;
	intframe* iframe;
	process_t* current = get_current_process();

	/* On récupère les données empilées par l'interruption */
	/* Le but ici est de remplacer ces valeurs par celles stockées dans la sigframe, 
	 * de cette manière l'iret à la fin de l'interruption restaurera le contexte du processus */
	iframe = (intframe*) (stack_ptr+2); /* XXX le "+4" a été déduis, pas très safe, faudrait chercher pourquoi */
	
	/* On récupère les données empilées avant le signal. */
	
	/* A la fin du handler, on pop l'adresse de retour, puis on pop eax, 
	 * esp se retrouve donc à +8 par rapport à la stack frame du signal, 
	 * on va donc chercher cette frame à esp-8
	 */	
	sframe = (sigframe*) (iframe->esp-8);

	
	iframe->eax = sframe->context.eax;
	iframe->ecx = sframe->context.ecx;
	iframe->edx = sframe->context.edx;
	iframe->ebx = sframe->context.ebx;
	iframe->kesp = sframe->context.kesp;
	iframe->ebp = sframe->context.ebp;
	iframe->esi = sframe->context.esi;
	iframe->edi = sframe->context.edi;
	iframe->eip = sframe->context.eip;
	iframe->eflags = sframe->context.eflags;
	iframe->cs = sframe->context.cs;
	iframe->ds = sframe->context.ds;
	iframe->es = sframe->context.es;
	iframe->fs = sframe->context.fs;
	iframe->gs = sframe->context.gs;
	
	if (sframe->context.cs == KERNEL_CODE_SEGMENT) {
		/* On remet esp à son origine */
		asm("mov %0, %%esp;"::"m" (iframe->kesp));
	} else {
		iframe->esp = sframe->context.esp;
		iframe->ss = sframe->context.ss;
	}

	current->signal_data.mask = sframe->mask;
	
	if(sframe->state == PROCSTATE_SUSPENDED)
		current->state = PROCSTATE_RUNNING;
	else
		current->state = sframe->state;
}

SYSCALL_HANDLER1(sys_sigsuspend, sigset_t* mask) {
	process_t* current = get_current_process();
	
	/* Sauvegarde du mask et de l'état du process */
	sigset_t old_mask = current->signal_data.mask;
	uint8_t	old_state = current->state;
	
	/* Utilisation du nouveau mask */
	current->signal_data.mask = *mask;
	
	/* Passage du processus en suspended */
	current->state = PROCSTATE_SUSPENDED;
	while(current->state == PROCSTATE_SUSPENDED);
	
	/* Restauration de l'état et du mask */
	current->state = old_state;
	current->signal_data.mask = old_mask;
}

static int get_first_signal(sigset_t* set)
{
	int i = 0;
	while(!(((*set)>>i)&0x1))
		i++;
	return i;
}

int signal_pending(process_t* process){
	return (process->signal_data.pending_set & (~process->signal_data.mask)) != 0;
}

int exec_sighandler(process_t* process)
{
	int signum;
	int ret = 0;	
	sigframe* frame;
	process_t* current_process = get_current_process();
		
	if(signal_pending(process))
	{
		/* On cherche le handler à exécuter */
		signum = get_first_signal(&(process->signal_data.pending_set));
		if(process->signal_data.handlers[signum] != NULL)
		{
			klog("Pid %d received signal %s.", process->pid, signal_names[signum]);
			ret = 1;
			
			/* On alloue sur la pile USER la stack frame du signal */
			if(process->regs.cs == USER_CODE_SEGMENT) {
				frame = (sigframe*)process->regs.esp;
				frame--;
			} else {
				frame = (sigframe*)(get_default_tss()->esp1);
				frame--;

			}
		
			if (current_process != process) {	
				/* Récupère l'adresse physique de répertoire de page du processus */
				paddr_t pd_paddr = vmm_get_page_paddr((vaddr_t) process->pd);
				/* On lui remet sa page directory */
				pagination_load_page_directory((struct page_directory_entry *)pd_paddr);
			}

			/* Et on remplis la frame avec ce qu'on veut */
			//frame->ret_addr = process->regs.eip;
			frame->ret_addr = (vaddr_t) frame->retcode;
			
			/* Le signal reçoit en argument le numero de signal */
			frame->sig = signum;
			
			/* Sauvegarde du contexte */
			frame->context.eax = process->regs.eax;
			frame->context.ecx = process->regs.ecx;
			frame->context.edx = process->regs.edx;
			frame->context.ebx = process->regs.ebx;
			frame->context.esp = process->regs.esp;
			frame->context.kesp = process->regs.kesp;
			frame->context.ebp = process->regs.ebp;
			frame->context.esi = process->regs.esi;
			frame->context.edi = process->regs.edi;
			frame->context.eip = process->regs.eip;
			frame->context.eflags = process->regs.eflags;
			frame->context.cs = process->regs.cs;
			frame->context.ss = process->regs.ss;
			frame->context.kss = process->regs.kss;
			frame->context.ds = process->regs.ds;
			frame->context.es = process->regs.es;
			frame->context.fs = process->regs.fs;
			frame->context.gs = process->regs.gs;
			frame->context.cr3 = process->regs.cr3;
			
			frame->mask = process->signal_data.mask;
			
			frame->state = process->state;
			
			/* So called "least-understood piece of the Linux kernel*/
			*((uint16_t*)(frame->retcode+0)) = 0xb858;
			*((uint32_t*)(frame->retcode+2)) = SYS_SIGRET;
			*((uint16_t*)(frame->retcode+6)) = 0x30cd;
			
			/* Mettre à jour esp */
			process->regs.esp = (uint32_t) frame;
			
			/* On fait pointer eip vers le handler du signal */
			process->regs.eip = (uint32_t) process->signal_data.handlers[signum];
			
			/* Et enfin, le gros piège: on doit absolument exécuter le handler en user space, donc on change cs: */
			process->regs.cs = USER_CODE_SEGMENT;
			process->regs.ds = USER_DATA_SEGMENT;
			process->regs.ss = USER_STACK_SEGMENT;
			
			process->state = PROCSTATE_RUNNING;
			
			sigaddset(&(process->signal_data.mask),signum);
		}
		
		sigdelset(&(process->signal_data.pending_set), signum);
	}
	
	return ret;
}

