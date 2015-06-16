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

#include "heap.h"
#include <kmalloc.h>
#include <klibc/string.h>

void heap_init(heap_t* l, cmp_func_type cmp, size_t elements_size, int max_elements) {
	l->comparator = cmp;
	l->nb_elements = 0;
	l->elements_array = kmalloc((max_elements + 1) * elements_size);
	l->max_elements = max_elements;
	l->elements_size = elements_size;
}

static inline void* get_element(heap_t* l, int i) {
	return l->elements_array + i * l->elements_size;
}

void* heap_top(heap_t *l) {
	if (l->nb_elements == 0) return NULL;
	return l->elements_array + l->elements_size;
}

void heap_remove(heap_t *l) {
	if (l->nb_elements == 0) return;

	memcpy(get_element(l, 1), get_element(l, l->nb_elements), l->elements_size);
	l->nb_elements--;

	int i = 1;
	while (1) {
		int left = 2 * i;
		int right = 2 * i + 1;
		int min = i;
		if (left <= l->nb_elements && l->comparator(get_element(l, left), get_element(l, min)) < 0) {
			min = left;
		}
		if (right <= l->nb_elements && l->comparator(get_element(l, right), get_element(l, min)) < 0) {
			min = right;
		}
		if (min != i) {
			memcpy(get_element(l, l->nb_elements + 1), get_element(l, i), l->elements_size);
			memcpy(get_element(l, i), get_element(l, min), l->elements_size);
			memcpy(get_element(l, min), get_element(l, l->nb_elements + 1), l->elements_size);
			i = min;
		} else {
			break;
		}
	}
}

void heap_push(heap_t *l, void* element) {
	l->nb_elements++;

	int i = l->nb_elements;
	int parent = i / 2;

	while (parent && l->comparator(element, get_element(l, parent)) < 0) {
		memcpy(get_element(l, i), get_element(l, parent), l->elements_size);

		i /= 2;
		parent /= 2;
	}

	memcpy(get_element(l, i), element, l->elements_size);
}
