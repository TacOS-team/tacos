/**
 * @file kmalloc.c
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

#include <memory.h>
#include <vmm.h>
#include <kmalloc.h>
#include <klibc/string.h>

#define ALIGN_BYTE(addr) (((uint32_t)(addr) + 7) & ~(7))

/**
 * Liste doublement chaînée de struct mem.
 */
struct mem_list
{
	struct mem *begin;
	struct mem *end;
};

/**
 * Chaque zone allouée débute par cette structure qui permet le chaînage (double)
 * dans les listes k_free_mem et k_allocated_mem.
 */
struct mem
{
	struct mem *prev; /**<  */
	size_t size; /**< taille (struct mem) + data */
	struct mem *next;
	uint32_t padding;
};

static struct virtual_mem kvm;
static struct mem_list k_free_mem;
static struct mem_list k_allocated_mem;

static int is_empty(struct mem_list *list);
static void remove(struct mem_list *list, struct mem *m);
static void add_first_element(struct mem_list *list, struct mem *m);
static void push_back(struct mem_list *list, struct mem *m);
//static void add(struct mem_list *list, struct mem *m);
static void cut_mem(struct mem* m, size_t size, 
										struct mem_list *free_mem, struct mem_list *allocated_mem);
static int is_stuck(struct mem *m1, struct mem *m2);

void init_kmalloc()
{
	init_vmm(&kvm);

	k_free_mem.begin = NULL;
	k_free_mem.end = NULL;
	k_allocated_mem.begin = NULL;
	k_allocated_mem.end = NULL;
}

void *kmalloc(size_t size)
{
	struct mem *mem = k_free_mem.begin;
	size_t real_size = ALIGN_BYTE(size + sizeof(struct mem));

	while(mem != NULL && mem->size < real_size)
		mem = mem->next;

	if(mem == NULL)
	{
		struct mem *new_mem;
		size_t real_alloc_size;

		real_alloc_size = allocate_new_pages(NULL, &kvm, calculate_min_pages(real_size),
																				 (void **) &new_mem, 0);
		
		if(real_alloc_size <= 0)
			return NULL;
		
		new_mem->size = real_alloc_size;
		push_back(&k_free_mem, new_mem);
		mem = k_free_mem.end;
	}

	cut_mem(mem, real_size, &k_free_mem, &k_allocated_mem);
	void * ret = (void *) (((uint32_t) mem) + sizeof(struct mem));

	// Uniquement pour éviter que certaines parties bugguées ne marche par chance.
	memset(ret, 0x42, size);

	return ret;
}

int kfree(void *p)
{
	struct mem *m = k_allocated_mem.end;

	while(m != NULL) {
		if((vaddr_t) m + sizeof(struct mem) == (vaddr_t) p)
			break;
		m = m->prev;
	}

	if(m == NULL)
		return -1;

	// Uniquement pour éviter que certaines parties bugguées ne marche par chance.
	memset(p, 0xdd, m->size - sizeof(struct mem));

	remove(&k_allocated_mem, m);
	//add(&k_free_mem, m);
	push_back(&k_free_mem, m);

	if(is_stuck(m->prev, m))
	{
		m->prev->size += m->size;
		remove(&k_free_mem, m);
		m = m->prev;
	}
	
	if(is_stuck(m, m->next))
	{
		m->size += m->next->size;
		remove(&k_free_mem, m->next);
	}

	return 0;
}

/* XXX ÜBER MOCHE, mais au moins ça doit marcher... */
void *kmalloc_one_aligned_page() {
	uint32_t tmp;
	allocate_new_pages(NULL, &kvm, 2, (void **) &tmp, 0);
	
	return (void *) ALIGN_PAGE_SUP(tmp);
}

static int is_empty(struct mem_list *list)
{
	return list->begin == NULL;
}

// Enlève une mem d'une mem_list
// Le bloc doit appartenir à la liste
static void remove(struct mem_list *list, struct mem *m)
{
	if(m->prev != NULL)
		m->prev->next = m->next;
	if(m->next != NULL)
		m->next->prev = m->prev;

	if(m == list->begin)
		list->begin = m->next;
	if(m == list->end)
		list->end = m->prev;
}

static void replace(struct mem_list *list, struct mem *m, struct mem *new_mem)
{
	new_mem->next = m->next;
	new_mem->prev = m->prev;

	if (new_mem->next) {
		new_mem->next->prev = new_mem;	
	} else {
		list->end = new_mem;
	}

	if (new_mem->prev) {
		new_mem->prev->next = new_mem;
	} else {
		list->begin = new_mem;
	}
}

static void add_first_element(struct mem_list *list, struct mem *m)
{
	list->begin = m;
	m->prev = NULL;
	m->next = NULL;
	list->end = m;
}

static void push_back(struct mem_list *list, struct mem *m)
{
	if (is_empty(list)) {
		add_first_element(list, m);
	} else {
		list->end->next = m;
		m->prev = list->end;
		m->next = NULL;
		list->end = m;
	}
}

/*
// Ajoute une mem à une mem_list
// l'ordre de la liste est celui de la mémoire virtuelle
static void add(struct mem_list *list, struct mem *m)
{
	struct mem *it = list->begin;

	if(is_empty(list))
	{
		add_first_element(list, m);
		return;
	}

	while(it->next != NULL && it->next < m)
		it = it->next;

	// add at the end of the list
	if(it->next == NULL)
	{
		list->end->next = m;
		m->prev = list->end;
		m->next = NULL;
		list->end = m;
		return;
	}

	m->prev = it;
	m->next = it->next;
	it->next->prev = m;
	it->next = m;
}
*/

// Sépare un bloc mem en deux quand il est trop grand et déplace le bloc de free_mem vers used_mem
static void cut_mem(struct mem* m, size_t size, 
										struct mem_list *free_mem, struct mem_list *allocated_mem)
{
	if(m->size - size > sizeof(struct mem)) // cut mem
	{
		struct mem *new_mem = (struct mem*) ((vaddr_t) m + size);
		new_mem->size = m->size - size;

		replace(free_mem, m, new_mem);

		m->size = size;
	} else {
		remove(free_mem, m);
	}
	push_back(allocated_mem, m);
//	add(allocated_mem, m);
}

static int is_stuck(struct mem *m1, struct mem *m2)
{
	return m1 != NULL && (vaddr_t) m1 + m1->size == (vaddr_t) m2;
}

