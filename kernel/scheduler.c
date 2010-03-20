#include <stdio.h>
#include <types.h>
#include <scheduler.h>
#include <process.h>
#include <events.h>

#define MAX_PROC 255
#if 0

struct process process_list[MAX_PROC];	// Tableau des processus en cours
//struct process idle;
uint8_t nb_proc;						// Nombre de processus en cours
uint8_t running_proc;					// Indice du processus tournant actuellement
uint32_t quantum;						// Quantum de temps alloué aux process

static int get_next_process()
{
	//
	// Tourniquet sans priorite
	//
	int ret = running_proc;
	
	ret++;
	if(ret >= nb_proc)
		ret = 0;
		
	while(process_list[ret].state == PROCSTATE_WAITING && ret!=running_proc)
	{
		int ret = running_proc;
	
		ret++;
		if(ret >= nb_proc)
		ret = 0;
	}
	
	if(ret == running_proc && process_list[ret].state == PROCSTATE_WAITING)
	{
		// tout les process sont en attente on fait idle
		ret == -1;
	}
	
	return ret;
}

static void* switch_process(void* data)
{
	// Mise en place de l'interruption sur le quantum de temps
	add_event(switch_process,NULL,quantum);
	
	// Choix du process suivant
	if(nb_proc>0)
	{
		struct process* current_proc;
		struct process* next_proc;
		int next_proc_id;
		
		// On recupere le processus actuel
		current_proc = &(process_list[running_proc]);
		
		// On recupere le prochain processus à executer	
		next_proc_id = get_next_process();
		
		if(next_proc_id == -1)
		{
			// On passe en idle
			running_proc = -1;
			asm("hlt");
		}
		else if(next_proc_id != running_proc)
		{
			running_proc = next_proc_id;
			// On doit switcher vers un autre process
			next_proc = &(process_list[running_proc]);
			// On bascule les drapeaux
			current_proc->state = PROCSTATE_IDLE;
			next_proc->state = PROCSTATE_RUNNING;
			// On change le contexte
			cpu_ctxt_switch(&current_proc->ctxt.esp, &next_proc->ctxt.esp);
		}
	}
	else
	{
		// On passe en idle
		running_proc = -1;
		asm("hlt");
	}
}
/*
int idle_func(int argc, char* argv[])
{
	asm("hlt");
}
*/

void init_scheduler(int qt, paddr_t init, uint32_t argc, uint8_t** argv)
{
	nb_proc = 1;
	running_proc = 0;
	quantum = qt;
	
	// Ajout du processus init
	add_process(init, argc, argv);

	// Mise en place de l'interruption sur le quantum de temps
	add_event(switch_process,NULL,quantum);
}

int add_process(paddr_t prog, uint32_t argc, uint8_t** argv)
{
	if(nb_proc < MAX_PROC)
	{
		struct process new_proc;
		
		if(init_process(prog, argc, argv, &new_proc) != 0)
		{
			return 2;
		}

		nb_proc++;		
		process_list[nb_proc] = new_proc;
		
		return 0;
	}
	
	return 1;
}
#endif
