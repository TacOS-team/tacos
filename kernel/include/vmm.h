/**
 * @file vmm.h
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
 * vmm est l'allocateur qui s'occupe de gérer les pages virtuelles du noyau.
 * Il se base sur l'algorithme first fit (premier emplacement de taille suffisante)
 *
 * Maintien la liste des espaces libres et des espaces occupés (slabs).
 *
 */

#ifndef _VMM_H_
#define _VMM_H_

#include <types.h>
#include <kprocess.h>

/**
 * @brief a slab of pages.
 *
 * chaque slab commence par un entête (struct slab) et est suivi par les données utiles.
 */
struct slab
{
	struct slab *prev;
	size_t nb_pages;
	struct slab *next;
	uint32_t padding;
};

struct slabs_list
{
	struct slab *begin;
	struct slab *end;
};

struct virtual_mem
{
	struct slabs_list free_slabs;
	struct slabs_list used_slabs;
	vaddr_t vmm_top;
};

/**
 * Initialisation de la VMM.
 *
 * @param kvm
 */
void init_vmm(struct virtual_mem *kvm);

void init_process_vm(process_t *process, struct virtual_mem *vm, int init_nb_pages);

/**
 * @brief Allocation d'une nouvelle page.
 *
 * @param vm
 * @param alloc
 * @param u_s 1 si accessible en userspace, 0 sinon.
 *
 * @return 
 */
unsigned int allocate_new_page(process_t* process, struct virtual_mem *vm, void **alloc, int u_s);

/**
 * @brief Alloue nb_pages pages qui sont placé en espace contigüe de la 
 * mémoire virtuelle.
 *
 * @param vm
 * @param nb_pages
 * @param alloc
 * @param u_s 1 si accessible en userspace, 0 sinon.
 *
 * @return
 */
unsigned int allocate_new_pages(process_t* process, struct virtual_mem *vm, unsigned int nb_pages,
																void **alloc, int u_s);
int unallocate_page(struct virtual_mem *vm, void *page);
unsigned int calculate_min_pages(size_t size);
void vmm_print_heap(struct virtual_mem *vm);
vaddr_t get_linear_address(int dir, int table, int offset);

/**
 * Syscall VMM pour allouer des pages.
 */
void sys_vmm(uint32_t min_size, void **alloc, size_t *real_alloc_size);

struct page_table_entry *get_pte(int dir, int table);
paddr_t vmm_get_page_paddr(vaddr_t vaddr); 

int map(paddr_t phys_page_addr, vaddr_t virt_page_addr, int u_s, int r_w);
void unmap(vaddr_t virt_page_addr);

#endif

