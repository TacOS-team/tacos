#ifndef SCHEDULER_H
#define SCHEDULER_H
/**
 * @file scheduler.h
 * 
 */
 
#include <process.h>
#include <ksyscall.h>

/* Structure à usage ultérieur, don't mind */
/* Cette structure doit contenir les differents pointeurs de fonctions pour manipuler le scheduler */
typedef struct {
	char* name;
	int (*initialize)(int);				/* Initialisation du scheduler */
	process_t* (*get_process)(int);		/* Trouver un process en fonction de son pid */
	process_t* (*get_next_process)();	/* Trouver le prochain processus selon le scheduler */
	process_t* (*get_current_process)();	/* Trouver le processus actuel */
	int (*add_process)(process_t*);		/* Ajouter un processus */
	int (*delete_process)(int);			/* Supprimer un processus */
	void (*inject_idle)(process_t*);
}scheduler_descriptor_t;

/**
 * @brief Initialisation du scheduler
 * Initialise les données du scheduler pour qu'il puisse être lancé correctement
 * 
 * @param Q Quantum du scheduler
 */ 
void init_scheduler(int Q);

/**
 * @brief Mise en route du scheduler
 * Met en route le scheduler en ajoutant la fonction schedule en évenement au temps Q (quantum)
 */
void start_scheduler();

/**
 * @brief Arret du scheduler
 * Arrète le scheduler en retirant le prochain schedule de la liste des évenements
 */
void stop_scheduler();

/**
 * @brief Handler de l'appel système sleep
 * Endors un processus, et crée un évenement pour le réveiller
 */
SYSCALL_HANDLER1(sys_sleep, uint32_t delay); 

SYSCALL_HANDLER0(sys_hlt);

void halt();
#endif //SCHEDULER_H
