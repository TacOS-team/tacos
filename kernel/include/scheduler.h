#ifndef SCHEDULER_H
#define SCHEDULER_H
/**
 * @file scheduler.h
 * 
 */
 
#include <process.h>

/* Structure à usage ultérieur, don't mind */
/* Cette structure doit contenir les differents pointeurs de fonctions pour manipuler le scheduler */
typedef struct {
	int (*initialize)(int);				/* Initialisation du scheduler */
	process_t* (*get_process)(int);		/* Trouver un process en fonction de son pid */
	process_t* (*get_next_process)();	/* Trouver le prochain processus selon le scheduler */
	process_t* (*get_current_process)();	/* Trouver le processus actuel */
	int (*add_process)(process_t*);		/* Ajouter un processus */
	int (*delete_process)();			/* Supprimer un processus */
}scheduler_descriptor;

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
void* sys_sleep( uint32_t delay,uint32_t unused2 __attribute__ ((unused)), uint32_t unused3 __attribute__ ((unused)));

#endif //SCHEDULER_H
