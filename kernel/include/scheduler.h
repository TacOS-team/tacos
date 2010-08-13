#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <process.h>

/* Cette structure doit contenir les differents pointeurs de fonctions pour manipuler le scheduler */
typedef struct {
	int (*initialize)(int);				/* Initialisation du scheduler */
	process_t* (*get_process)(int);		/* Trouver un process en fonction de son pid */
	process_t* (*get_next_process)();	/* Trouver le prochain processus selon le scheduler */
	process_t* (*get_current_process)();	/* Trouver le processus actuel */
	int (*add_process)(process_t*);		/* Ajouter un processus */
	int (*delete_process)();			/* Supprimer un processus */
}scheduler_descriptor;

void init_scheduler(int Q);
void start_scheduler();
void stop_scheduler();

void* sys_exec(paddr_t prog, char* name, uint32_t unused __attribute__ ((unused)));
void exec(paddr_t prog, char* name);

void* sys_sleep( uint32_t delay,uint32_t unused2 __attribute__ ((unused)), uint32_t unused3 __attribute__ ((unused)));

#endif //SCHEDULER_H
