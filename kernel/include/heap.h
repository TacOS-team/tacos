/**
 * @file heap.c
 *
 * @author TacOS developers 
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

#ifndef _HEAP_H_
#define _HEAP_H_

#include <types.h>

typedef int (*cmp_func_type) (void*, void*);

/**
 * @brief Liste générique.
 */
typedef struct {
	cmp_func_type comparator; /**< Fonction de comparaison des elements */

	void* elements_array; 	/**< Tableau contenant les éléments */
	
	int nb_elements; 		/**< Nombre d'éléments présents dans la tas */
	size_t elements_size; 	/**< Taille du type d'élement à contenir */
	int max_elements; 		/**< Nombre maximum d'élements dans le tas */
} heap_t;

void heap_init(heap_t* l, cmp_func_type cmp, size_t elements_size, int max_elements);
void* heap_top(heap_t *l);
void heap_remove(heap_t *l);
void heap_push(heap_t *l, void* element);

#endif
