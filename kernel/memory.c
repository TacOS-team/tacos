/**
 * @file memory.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2014 TacOS developers.
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
 * @brief Gestion des pages mémoires physiques. 
 */

#include <memory.h>
#include <klog.h>

static struct physical_page_descr *used_frame_pages = NULL;
static struct physical_page_descr *free_frame_pages = NULL;

/* C'est les marqueurs qui indiquent où commence le kernel et où il se fini en mémoire. */
extern char _start, __e_kernel;

static struct physical_page_descr * phys_page_descr_array;
static paddr_t kernel_top;

void memory_print_used_pages() {
	struct physical_page_descr *p;
	int c = 0;

	kprintf("Used pages : ");

	p = used_frame_pages;
	while (p != NULL) {
		kprintf("%x ", p->addr);
		p = p->next;
		c++;
	}
	kprintf(" (%d pages) \n", c);
}

void memory_print_free_pages() {
	struct physical_page_descr *p;

	kprintf("Free pages : ");

	p = free_frame_pages;
	while (p != NULL) {
		kprintf("%x ", p->addr);
		p = p->next;
	}
	kprintf("\n");
}

int memory_get_total() {
	return memory_get_used() + memory_get_free();
}

int memory_get_used() {
	int count = 0;
	struct physical_page_descr *p;
	p = used_frame_pages;
	while (p != NULL) {
		count++;
		p = p->next;
	}
	return count * PAGE_SIZE;
}

int memory_get_free() {
	int count = 0;
	struct physical_page_descr *p;
	p = free_frame_pages;
	while (p != NULL) {
		count++;
		p = p->next;
	}
	return count * PAGE_SIZE;
}


void memory_print() {
	struct physical_page_descr *p_f, *p_u;

	p_f = free_frame_pages;
	p_u = used_frame_pages;
	while (p_f != NULL || p_u != NULL) {
		while ((p_f != NULL && p_u == NULL) || (p_f != NULL && p_f->addr > p_u->addr)) {
			kprintf("\033[34mF\033[0m");
			p_f = p_f->next;
		}
		while ((p_u != NULL && p_f == NULL) || (p_u != NULL && p_u->addr > p_f->addr)) {
			kprintf("\033[31mU\033[0m");
			p_u = p_u->next;
		}
	}
	kprintf("\n");
}

struct physical_page_descr * memory_get_first_used_page() {
	return used_frame_pages;
}

struct physical_page_descr * memory_get_first_free_page() {
	return free_frame_pages;
}

bool memory_has_next_page(struct physical_page_descr * iterator) {
	return (iterator != NULL);
}

paddr_t memory_next_page(struct physical_page_descr ** iterator) {
	if (iterator != NULL) {
		paddr_t r = (*iterator)->addr;
		*iterator = (*iterator)->next;
		return r;
	} else {
		return 0;
	}
}

/*
 * Découpage de la mémoire physique en taille fixe. Pour l'instant il n'y a pas de mémoire 
 * virtuelle, c'est donc juste une association linéaire avec autant de pages
 * que de cadres.
 */
void memory_setup(size_t ram_size) {
	ram_size = ALIGN_PAGE_INF(ram_size); /* On abandonne un petit bout de mémoire pour avoir que des pages completes */

	/* On place le tableau de pages juste après la fin du kernel. */
	phys_page_descr_array = (struct physical_page_descr*) ALIGN_PAGE_SUP((size_t)(& __e_kernel));

	size_t phys_mem_base = PAGE_SIZE; /* La page d'adresse 0 est réservée pour les erreurs. (c'est un choix arbitraire) */
	size_t phys_mem_top = ram_size;

	/* Maintenant on va récupérer les pages qui chevauchent l'emplacement où est le
	 * kernel ainsi que la table qui contient les pages. On va les marquer utilisées. */
	paddr_t kernel_base = (paddr_t) ALIGN_PAGE_INF(& _start);
	uint32_t n_pages = ram_size / PAGE_SIZE; /* ram_size est un multiple de PAGE_SIZE ! */
	kernel_top = (paddr_t)phys_page_descr_array + ALIGN_PAGE_SUP(n_pages * sizeof(struct physical_page_descr));

	struct physical_page_descr *phys_page_descr = (struct physical_page_descr*) phys_page_descr_array;
	paddr_t phys_page_addr = phys_mem_base; /* Adresse d'une page */

	/* On va initialiser le tableau de descripteur de pages. */
	while (phys_page_addr < phys_mem_top) {
		phys_page_descr->addr = phys_page_addr;

		/* Si la zone mémoire est déjà utilisée par le kernel ou par le BIOS alors on marque les pages comme 
		 * étant déjà utilisées pour ne pas les attribuer. */
		if ((phys_page_addr >= BIOS_RESERVED_BASE && phys_page_addr < BIOS_RESERVED_TOP)
				|| (phys_page_addr >= kernel_base && phys_page_addr < kernel_top)) {
			/* TODO : Ajouter des méthodes de gestion d'une liste doublement chainée... */
			phys_page_descr->next = used_frame_pages;
			phys_page_descr->prev = NULL;
			if (used_frame_pages != NULL) {
				used_frame_pages->prev = phys_page_descr;
			}
			used_frame_pages = phys_page_descr;
		} else {
			phys_page_descr->next = free_frame_pages;
			phys_page_descr->prev = NULL;
			if (free_frame_pages != NULL) {
				free_frame_pages->prev = phys_page_descr;
			}
			free_frame_pages = phys_page_descr;
		}

		/* On passe à la page suivante ! */
		phys_page_descr++;
		phys_page_addr += PAGE_SIZE;
	}
}

paddr_t memory_reserve_page_frame()
{
	struct physical_page_descr *p = free_frame_pages;

	if (p != NULL) {
		// pop it from free page stack
		free_frame_pages = free_frame_pages->next;
		if(free_frame_pages != NULL)
			free_frame_pages->prev = NULL;

		// put the new reserved on used page stack
		used_frame_pages->prev = p;
		p->next = used_frame_pages;
		used_frame_pages = p;
		p->prev = NULL;

		return p->addr;
	} else {
		return 0;
	}
}

int memory_free_page_frame(paddr_t addr)
{
	int found = 0;
	struct physical_page_descr *p;

	p = used_frame_pages;
	while (p != NULL)
	{
		if(p->addr == addr)
		{
			found = 1;
			if(p->next != NULL)
				p->next->prev = p->prev;
			if(p->prev != NULL)
				p->prev->next = p->next;

			break;
		}
	}
	
	if(!found)
		return -1;

	used_frame_pages = used_frame_pages->next;

	if(free_frame_pages != NULL) 
		free_frame_pages->prev = p;
	p->prev = NULL;
	p->next = free_frame_pages;
	free_frame_pages = p;

	return 0;
}

paddr_t memory_get_kernel_top() {
	return kernel_top + PAGE_SIZE;
}

