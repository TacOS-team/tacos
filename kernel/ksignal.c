#include <ksignal.h>
#include <kprocess.h>

/*
 * TODO: gérer SIG_IGN et SIG_DFL 
 */
void sys_signal(uint32_t param1, uint32_t param2, uint32_t param3)
{
	uint32_t signum = param1;
	sighandler_t handler = (sighandler_t) param2;
	sighandler_t* ret = param3;
	
	process_t* current = get_current_process();

	if(signum <= NSIG)
	{
		current->signal_data.handlers[signum] = handler;
		*ret = handler;
	}
}

void sys_sigprocmask(uint32_t param1, uint32_t param2, uint32_t param3)
{
	uint32_t how	= param1;
	sigset_t* set	= (sigset_t*) param2;
	sigset_t* oldset	= (sigset_t*) param3;
	
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
			kprintf("Invalid sigprocmask commande.\n");
	}
} 

/* TODO: d'après POSIX, kill(-1, sig) doit envoyer le signal à tous les processus possibles... */
void sys_kill(int pid, int signum, int* ret)
{	
	process_t* process = find_process(pid);
	
	*ret = -1;
	
	if(process != NULL)
	{
		*ret = sigaddset( &(process->signal_data.pending_set), signum );
	}
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
			ptr = process->regs.esp;
			
			/* Empiler l'argument */
			/*ptr--;
			ptr = signum;*/
			
			/* Empiler l'adresse de retour */
			ptr--;
			*ptr = process->regs.eip;
			
			/* Mettre à jour esp */
			process->regs.esp = ptr;
			
			/* Mettre à jour eip */
			process->regs.eip = process->signal_data.handlers[signum];
		}
		
		sigdelset(&(process->signal_data.pending_set), signum);
	}
}
