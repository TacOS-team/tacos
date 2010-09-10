/**
 * @file vmm.h
 *
 * @author TacOS developers 
 *
 * Maxime Cheramy <maxime81@gmail.com>
 * Nicolas Floquet <nicolasfloquet@gmail.com>
 * Benjamin Hautbois <bhautboi@gmail.com>
 * Ludovic Rigal <ludovic.rigal@gmail.com>
 * Simon Vernhes <simon@vernhes.eu>
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

#ifndef _VMM_H_
#define _VMM_H_

#include <types.h>

// a slab of pages
// chaque slab commence par un entête (struct slab) et est suivi par les données utiles.
struct slab
{
	struct slab *prev;
	size_t nb_pages;
	struct slab *next;
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

void init_vmm(struct virtual_mem *kvm);
vaddr_t init_process_vm(struct virtual_mem *vm, int init_nb_pages);
unsigned int allocate_new_page(struct virtual_mem *vm, void **alloc);
unsigned int allocate_new_pages(struct virtual_mem *vm, unsigned int nb_pages,
																void **alloc);
int unallocate_page(struct virtual_mem *vm, void *page);
unsigned int calculate_min_pages(size_t size);
void vmm_print_heap(struct virtual_mem *vm);
vaddr_t get_linear_address(int dir, int table, int offset);
void sys_vmm(uint32_t vm, uint32_t nb_pages, uint32_t alloc);
int map(paddr_t phys_page_addr, vaddr_t virt_page_addr);
struct page_table_entry *get_pte(int dir, int table);
paddr_t vmm_get_page_paddr(vaddr_t vaddr); 

#endif

