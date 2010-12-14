/**
/**
 * @file ksignal.c
 *
 * @author TacOS developers 
 *
 * Maxime Cheramy <maxime81@gmail.com>
 * Nicolas Floquet <nicolasfloquet@gmail.com>
 * Benjamin Hautbois <bhautboi@gmail.com>
 * Ludovic Rigal <ludovic.rigal@gmail.com>
 * Simon Vernhes <simon@vernhes.eu>
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

#include <ksignal.h>
#include <kprocess.h>
#include <kstdio.h>
#include <scheduler.h>
#include <klog.h>

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
			current->signal_data.mask |= (*set);
			break;
		case SIG_BLOCK:
			current->signal_data.mask &= ~(*set);
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
	}
	
	if(ret!=NULL)
		*ret = retour;
}

static int get_first_signal(sigset_t* set)
{
	int i = 0;
	while(!(((*set)>>i)&0x1))
		i++;
	return i;
}

int exec_sighandler(process_t* process)
{
	int signum;
	uint32_t* ptr;
	int ret = 0;
	if(process->signal_data.pending_set != 0)
	{
		/* On cherche le handler à exécuter */
		signum = get_first_signal(&(process->signal_data.pending_set));
		if(process->signal_data.handlers[signum] != NULL)
		{
			ret = 1;
			ptr = (uint32_t*)process->regs.esp;
			
			/* Empiler l'argument */
			/*ptr--;
			ptr = signum;*/
			
			/* Empiler l'adresse de retour */
			ptr--;
			*ptr = process->regs.eip;
			
			/* Mettre à jour esp */
			process->regs.esp = (uint32_t) ptr;
			
			/* Mettre à jour eip */
			process->regs.eip = (uint32_t) process->signal_data.handlers[signum];
		}
		
		sigdelset(&(process->signal_data.pending_set), signum);
	}
	
	return ret;
}
