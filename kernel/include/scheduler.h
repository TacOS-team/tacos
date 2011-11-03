/**
 * @file scheduler.h
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

#ifndef SCHEDULER_H
#define SCHEDULER_H
/**
 * @file scheduler.h
 * 
 */
 
#include <kprocess.h>
#include <ksyscall.h>

/* Cette structure doit contenir les differents pointeurs de fonctions pour manipuler le scheduler */
typedef struct {
	char* name;
	int (*initialize)(int);				/* Initialisation du scheduler */
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
void set_scheduler(scheduler_descriptor_t* sched);
int scheduler_add_process(process_t* proc);
int scheduler_delete_process(int pid);

/** 
* @brief Retourne le processus en cours d'utilisation
* 
* @return processus en cours d'utilisation.
*/
process_t* get_current_process();


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

int is_schedulable(process_t* process);

void halt();
#endif //SCHEDULER_H
