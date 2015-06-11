/**
 * @file round_robin.c
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

#include <kmalloc.h>
#include <kprocess.h>
#include <scheduler.h>

/*
 * PROTOTYPES
 */
static process_t* rr_get_next_process();
static int rr_add_process(process_t* proc);
static int rr_delete_process(int pid);

/*
 * Structure à exporter pour utiliser le code en tant que module scheduler
 */
scheduler_descriptor_t round_robin = {
	.name = "Round robin",
	.initialize = NULL,
	.get_next_process = rr_get_next_process,
	.add_process = rr_add_process,
	.delete_process = rr_delete_process
};

typedef struct _list_cell
{
	 process_t* proc; 
	 struct _list_cell* prev;
	 struct _list_cell* next;
	 
} list_cell; /**< Cellule de la liste doublement chainée. */

int is_idle = 1; /**< 1 si actuellement aucun processus ne tourne. */
static list_cell* process_list = NULL; /**< Liste chainée circulaire des process. */
static list_cell* current_cell = NULL; /**< Pointeur sur le dernier process exécuté. */

static process_t* rr_get_next_process()
{
	list_cell* aux = current_cell->next;
	list_cell* first = aux;
	do {
		if (is_schedulable(aux->proc))
			current_cell = aux;
		else
			aux = aux->next;
	} while (aux != current_cell && aux != first);

	if (is_schedulable(aux->proc))
		is_idle = 0;
	else
		is_idle = 1;

	return aux->proc;
}

static int rr_add_process(process_t* proc)
{
	list_cell* aux;
	if(process_list == NULL)
	{
		process_list = kmalloc(sizeof(list_cell));
		process_list->next = process_list;
		process_list->prev = process_list;
		process_list->proc = proc;
		current_cell = process_list;
	}
	else
	{
		list_cell* last = process_list->prev;

		aux = kmalloc(sizeof(list_cell));
		aux->prev = last;
		aux->next = process_list;
		aux->proc = proc;
		process_list->prev = aux;
		process_list = aux;
		last->next = aux;
	}
	return 0;
}

static int rr_delete_process(int pid)
{
	list_cell* aux = process_list;
	int ret = -1;
	
	while(aux->next != process_list && aux->proc->pid != pid)
		aux = aux->next;
	
	if(aux != NULL)
	{
		ret = 1;
		
		/* si aux est la tête de la liste */
		if (aux == process_list) {
			process_list = aux->next;
		}

		aux->prev->next = aux->next;
		aux->next->prev = aux->prev;

		if (current_cell == aux) {
			current_cell = aux->prev;
			is_idle = 1;
		}

		kfree(aux);
	}

	return ret;
}
