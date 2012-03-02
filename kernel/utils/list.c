/**
 * @file list.c
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
 * Description de ce que fait le fichier
 */

/*
 * TESTS UNITAIRES OK
 */

#include <list.h>
#include <string.h>
#include <kmalloc.h>
#include <kstdio.h>

#define EMPTY_SLOT -1
#define END_SLOT -2

void list_init(list_t * l, cmp_func_type cmp, size_t elements_size,
	       int max_elements)
{
	int i;

	l->comparator = cmp;
	l->nb_elements = 0;
	l->max_elements = max_elements;
	l->elements_array = kmalloc(max_elements * elements_size);
	l->link_array = kmalloc(max_elements * sizeof(int));

	for (i = 0; i < max_elements; i++)
		l->link_array[i] = EMPTY_SLOT;
	l->head = EMPTY_SLOT;

	l->elements_size = elements_size;
}

static inline void *get_element(list_t * l, int index)
{
	return l->elements_array + (index * l->elements_size);
}

void *list_get_top(list_t l)
{
	void *top = NULL;

	if (l.nb_elements > 0)
		top = get_element(&l, l.head);

	return top;
}

int list_add_element(list_t * l, void *element)
{

	int ret = 1;
	int i = 0, j = 0;

	if (l->nb_elements < l->max_elements) {
		/* Insertion de l'élément dans le tableau de données */
		while (l->link_array[i] != EMPTY_SLOT)
			i++;
		memcpy(get_element(l, i), element, l->elements_size);

		/* Si la liste est vide */
		if (l->nb_elements == 0) {
			l->link_array[i] = END_SLOT;
			l->head = i;
		}
		/* Si l'élément se place en tête */
		else if (l->comparator(element, get_element(l, l->head)) <= 0) {
			l->link_array[i] = l->head;
			l->head = i;
		} else {
			j = l->head;
			/* On cherche la position du nouvel élément */

			while (l->link_array[j] != END_SLOT
			       && l->comparator(element,
						get_element(l,
							    l->link_array[j])) >
			       0) {
				j = l->link_array[j];
			}

			/* Et on fait le lien */
			l->link_array[i] = l->link_array[j];
			l->link_array[j] = i;

		}
		l->nb_elements++;

		ret = 0;
	}
	return ret;
}

int list_remove_top(list_t * l)
{
	int ret = 1;
	int head = l->head;
	if (l->nb_elements > 0) {
		l->head = l->link_array[head];
		l->link_array[head] = EMPTY_SLOT;
		l->nb_elements--;
		ret = 0;
	}

	return ret;
}

void print_list(list_t ma_liste)
{
	int a;
	a = ma_liste.head;
	int *i;
	while (a != END_SLOT) {
		i = get_element(&ma_liste, a);
		kprintf("%d ", *i);
		a = ma_liste.link_array[a];
	}
}

int list_del_element(list_t * list, int id, id_func_type func)
{
	int el_index = 0;
	int i = 0;

	while (!func(id, get_element(list, el_index)))
		el_index++;

	while (list->link_array[i] != el_index)
		i++;

	list->link_array[i] = list->link_array[el_index];

	list->link_array[el_index] = EMPTY_SLOT;

	list->nb_elements--;

	return 0;		/* TODO: bonne valeur de retour */
}
