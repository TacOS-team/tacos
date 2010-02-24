#include <stdio.h>
#include <types.h>
#include <scheduler.h>
#include <process.h>

#define MAX_PROC 255

struct process process_list[MAX_PROC];	// Tableau des processus en cours
uint8_t nb_proc;						// Nombre de processus en cours
uint8_t running_proc;					// Indice du processus tournant actuellement
uint32_t quantum;						// Quantum de temps alloué aux process

void init_scheduler(int qt)
{
	nb_proc = 0;
	running_proc = 0;
	quantum = qt;
	
	// Mise en place de l'interruption sur le quantum de temps
	// TODO
}

int get_next_process()
{
	//
	// Tourniquet sans priorite
	//
	
	int ret = running_proc;
	ret++;
	
	if(ret >= nb_proc)
		ret = 0;
		
	return ret;
}

void switch_process()
{
	if(nb_proc>0)
	{
		struct process* current_proc;
		struct process* next_proc;
	
		// On recupere le processus actuel
		current_proc = &(process_list[running_proc]);
		
		// On recupere le prochain processus à executer	
		running_proc = get_next_process();
		next_proc = &(process_list[running_proc]);

		// On bascule les drapeaux
		current_proc->state = PROCSTATE_IDLE;
		next_proc->state = PROCSTATE_RUNNING;
		// On change le contexte
		cpu_ctxt_switch(&current_proc->ctxt.esp, &next_proc->ctxt.esp);
	}
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
