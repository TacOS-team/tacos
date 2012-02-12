/**
 * @file list.h
 *
 * @author TacOS developers 
 *
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
 * Description de ce que fait le fichier
 */

#ifndef _LIST_H_
#define _LIST_H_

#include <types.h>

typedef int (*cmp_func_type) (void*, void*);
typedef int (*id_func_type) (int, void*);

typedef struct {
	
	cmp_func_type comparator; /* Fonction de comparaison des elements */
	
	int head; 				/* Indice de la tête de liste */
	void* elements_array; 	/* Tableau contenant les éléments */
	int* link_array; 		/* Tableau contenant les liens entre les éléments,sers aussi à savoir si un emplacement est vide */
	
	int nb_elements; 		/* Nombre d'éléments présents dans la liste */
	size_t elements_size; 	/* Taille du type d'élement à contenir */
	int max_elements; 		/* Nombre maximum d'élements dans la liste */
} list_t;

void initList(list_t* l, cmp_func_type cmp, size_t elements_size,int max_elements);
void* listGetTop(list_t l);
int listAddElement(list_t* l, void* element);
int listRemoveTop(list_t* l);
int listDelElement(list_t* list, int id, id_func_type func);

void print_list(list_t ma_liste);

#endif //_LIST_H_
