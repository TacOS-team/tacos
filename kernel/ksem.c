/**
 * @file ksem.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012 - TacOS developers.
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
 * @brief Gestion des sémaphores.
 */

/* TODO: Séparer le code de la fifo dans un autre fichier, voir le faire générique pour que ce soit réutilisable pour d'autres types d'IPC */

#include <ksem.h>
#include <kprocess.h>
#include <kmalloc.h>
#include <scheduler.h>

#define FIFO_MAX_SIZE 32
#define MAX_SEM 255
#define KSEM_GET 1
#define KSEM_CREATE 2
#define KSEM_DEL 3
#define KSEM_P 4
#define KSEM_V 5
#define SYS_SEMCTL 8

/***********************************
 * 
 * STRUCT DECLARATIONS
 * 
 ***********************************/

/**
 * Cellule d'une fifo.
 */
typedef struct _sem_fifo_cell{
	int pid;
	struct _sem_fifo_cell* prev;
	struct _sem_fifo_cell* next;
} sem_fifo_cell;

/**
 * Structure d'une fifo.
 */
typedef struct {
	int size;
	sem_fifo_cell* head;
	sem_fifo_cell* tail;
} sem_fifo;

/**
 * Structure définissant une sémaphore.
 */
typedef struct {
	int value;
	uint8_t allocated;
	sem_fifo fifo;
} sem_t;

/************************************
 * 
 * GLOBAL DECLARATIONS
 * 
 ************************************/

static sem_t semaphores[MAX_SEM];

/*************************************
 * 
 * CODE
 * 
 *************************************/

/**
 *  Initialisation de la fifo 
 */
static void sem_fifo_init(sem_fifo* fifo)
{
	fifo->size = 0;
	fifo->head = NULL;
	fifo->tail = NULL;
}

/**
 *  Met un élément dans la file 
 */
static int sem_fifo_put(sem_fifo* fifo, int pid)
{
	int ret = -1;
	sem_fifo_cell* new_cell;
	if(fifo->size < FIFO_MAX_SIZE)
	{
		new_cell = (sem_fifo_cell*) kmalloc(sizeof(sem_fifo_cell));
		new_cell->pid = pid;
		new_cell->prev = NULL;
		
		if(fifo->size == 0)
		{
			new_cell->next = NULL;
			fifo->tail = new_cell;
		}
		else
		{
			new_cell->next = fifo->head;
			fifo->head->prev = new_cell;
		}

		fifo->head = new_cell;
		
		fifo->size ++;
		ret = 0;
	}
	return ret;
}
 
/**
 *  Prend le prochain élément dans la file 
 */
static int sem_fifo_get(sem_fifo* fifo)
{
	int pid = -1;
	sem_fifo_cell* temp = fifo->tail;
	if(fifo->size > 0)
	{
		fifo->tail = temp->prev;
		pid = temp->pid;
		kfree(temp);
		fifo->size--;
	}
	return pid;
}

/**
 * Retourne la taille actuelle de la fifo
 */
static int sem_fifo_size(sem_fifo* fifo)
{
	return fifo->size;
}

/**
 * Revoie une clef libre.
 */
static uint8_t ksem_getkey()
{

  uint8_t new_key = 0;

	while (new_key < MAX_SEM && semaphores[new_key].allocated) {
		new_key++;
	}
  
	return new_key;
}


int init_semaphores()
{
	int i;
	for (i = 0; i < MAX_SEM; i++) {
		semaphores[i].value = 0;
		semaphores[i].allocated = 0;
		sem_fifo_init(&(semaphores[i].fifo));
	}
	return 0;
}

/**
 * @return key en cas de succès, -1 sinon.
 */
int ksemget(uint8_t key, int flags) {
	int ret = -1;

	if (key == SEM_NEW && (flags & SEM_CREATE)) {
		key = ksem_getkey();
	}
	if (key < MAX_SEM && !semaphores[key].allocated)
	{
		semaphores[key].allocated = 1;
		semaphores[key].value = 1;
		ret = key;
	}

	return ret;
}

int ksemctl(uint8_t key, int cmd, void* res) {
	sem_t* sem = &semaphores[key];
	if (sem->allocated == 0) {
		return -1;
	}

	switch (cmd) {
		case SEM_GETVAL:
			*((int*)res) = sem->value;
			break;
		case SEM_DEL:
			while (sem_fifo_size(&(sem->fifo)) > 0) {
				process_t *proc = find_process(sem_fifo_get(&(sem->fifo)));
				proc->state = PROCSTATE_RUNNING;
			}
			sem->allocated = 0;
			break;
		case SEM_ZCNT:
			*((int*)res) = sem_fifo_size(&(sem->fifo));
			break;
		case SEM_SET:
			sem->value = *((int*)res);
			break;
		default:
			return -1;
	}
	return 0;
}

int ksemP(uint8_t key)
{
	int ret = -1;
	sem_t* sem = &semaphores[key];
	
	if (sem->allocated) {	
		/* Si le sémaphore est libre, on le prend, sinon, on attend */
		if(sem->value >= 1) {
			sem->value--;
		} else {
			process_t* proc = get_current_process();
			/* Si on attend, on met le pid dans la file d'attente pour pouvoir traiter le processus quand le semaphore sera libre */
			sem_fifo_put(&(sem->fifo), proc->pid);
			proc->state = PROCSTATE_WAITING;
			while(proc->state == PROCSTATE_WAITING) asm("hlt");
		}

		ret = 0;
	}
	
	return ret;
}

int ksemV(uint8_t key)
{
	int ret = -1;
	sem_t* sem = &semaphores[key];
	
	if (sem->allocated) {
		/* Si la file est vide, on incrémente la valeur, sinon, on débloque le premier processus en attente */
		if (sem_fifo_size(&(sem->fifo)) == 0) {
			sem->value++;
		} else {
			process_t *proc = find_process(sem_fifo_get(&(sem->fifo)));
			proc->state = PROCSTATE_RUNNING;
		}
		ret = 0;
	}
	return ret;
}


SYSCALL_HANDLER3(sys_ksem, uint32_t op, uint32_t param, int *ret) {
	switch(op)
	{
		case KSEM_GET:
			*ret = ksemget(param, *ret);
			break;
		case KSEM_P:
			*ret = ksemP(param);
			break;
		case KSEM_V:
			*ret = ksemV(param);
			break;
		default:
			break;
	}	
}

