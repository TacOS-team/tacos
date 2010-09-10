/**
 * @file pagination.c
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

/**
 * @file pagination.c
 */

#include <types.h>
#include <memory.h>
#include <pagination.h>
#include <vmm.h>
#include <stdio.h>

void pagination_map(struct page_directory_entry * pagination_kernel, paddr_t page_addr, vaddr_t v_page_addr);

void pagination_create_page_dir(struct page_directory_entry *pagination_kernel, 
																int index_pd) {
	int i;
	struct page_directory_entry * pde = &pagination_kernel[index_pd];
	paddr_t pt_addr = memory_reserve_page_frame();
	pde->r_w = 1;
	pde->u_s = 1;
	pde->present = 1;
	pde->page_table_addr = pt_addr >> 12;

	struct page_table_entry *pt = (struct page_table_entry *) pt_addr;
	for (i = 0; i < 1024; i++)
		pt[i].present = 0;

	// On map la table de page
	pagination_map(pagination_kernel, pt_addr, get_page_table_vaddr(index_pd));
}

void pagination_map(struct page_directory_entry * pagination_kernel, paddr_t page_addr, vaddr_t v_page_addr) {
	int index_pd = v_page_addr >> 22;
	int index_pt = (v_page_addr & 0x003FF000) >> 12;

	struct page_directory_entry * pde = &pagination_kernel[index_pd];
	if (!pde->present) {
		pagination_create_page_dir(pagination_kernel, index_pd);
	}

	struct page_table_entry * page_table = (struct page_table_entry *) (pde->page_table_addr << 12);
	struct page_table_entry * pte = &page_table[index_pt];

	pte->present = 1;
	pte->page_addr = page_addr >> 12;
	pte->r_w = 1;
	pte->u_s = 1;
}

/** 
 * @brief Mappe une adresse en identity mapping.
 * 
 * Cette fonction sert à mapper une adresse par identity mapping. Cela ne peut
 * être fait qu'avant l'activation de la pagination !
 *
 * @param pagination_kernel
 * @param page_addr
 */
void pagination_identity_map_addr(struct page_directory_entry * pagination_kernel, paddr_t page_addr) {
	pagination_map(pagination_kernel, page_addr, page_addr);
}

static void pagination_create_page_table_for_kernel(struct page_directory_entry *pagination_kernel) {
	int i;	
	int kernel_top_last_dir_entry = _PAGINATION_KERNEL_TOP >> 22;

	for(i=0 ; i<kernel_top_last_dir_entry ; i++) {
		if(!pagination_kernel[i].present) {
			pagination_create_page_dir(pagination_kernel, i);
		}
	}
}

void pagination_setup() {
	struct physical_page_descr * iterator = memory_get_first_used_page();
	struct page_directory_entry * pagination_kernel = (struct page_directory_entry*) memory_reserve_page_frame();

	// Emplacement en mémoire virtuelle du rep de page.
	// Placé en haut de la mémoire virtuelle (les 1024 premières sont
	// utilisé par le rep de page pour pointer vers lui-même.
	// les 1024 suivantes sont les emplacements réservers aux tables de pages
	// Et enfin on map le rep de page en dessous de tout celà.
	vaddr_t page_dir_vaddr = -2049*PAGE_SIZE;

	pagination_init_page_directory(pagination_kernel);
	pagination_map(pagination_kernel, (paddr_t) pagination_kernel,
								 page_dir_vaddr);

	// identity mapping :
	paddr_t current_page;
	while (memory_has_next_page(iterator)) {
		current_page = memory_next_page(&iterator);
		if (pagination_kernel != (struct page_directory_entry*) current_page) {
			pagination_identity_map_addr(pagination_kernel, current_page);
		}
	}

	pagination_create_page_table_for_kernel(pagination_kernel);

	pagination_load_page_directory(pagination_kernel);

	//reads cr0, switches the "paging enable" bit, and writes it back.
	unsigned int cr0;
	asm volatile("mov %%cr0, %0": "=b"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0":: "b"(cr0));
	
}

void pagination_init_page_directory(struct page_directory_entry * pd) {
	int i;

	for (i = 0; i < 1023; i++) {
		pd[i].present = 0;
	}

	// La dernière entrée pointe vers lui même ce qui fait que :
	// 0xFFFFF00 est traduit par la MMU en l'adresse du page directory
	// 0xFFC + 1024 * index_page_table permet d'accéder à une pde
	pd[1023].page_table_addr = ((paddr_t)pd) >> 12;
	pd[1023].r_w = 1;
	pd[1023].present = 1;
	pd[1023].u_s = 1;
}

void pagination_init_page_directory_copy_kernel_only(struct page_directory_entry *pd, paddr_t pd_paddr) {
	int i;
	struct page_directory_entry * current = 
		(struct page_directory_entry *)0xFFFFF000;
	
	int kernel_top_last_dir_entry = _PAGINATION_KERNEL_TOP >> 22;

	for (i = 0; i < kernel_top_last_dir_entry; i++) {
		pd[i].present = current[i].present;
		pd[i].r_w = current[i].r_w;
		pd[i].u_s = current[i].u_s;
		pd[i].pwt = current[i].pwt;
		pd[i].pcd = current[i].pcd;
		pd[i].a = current[i].a;
		pd[i].ign1 = current[i].ign1;		
		pd[i].ps = current[i].ps;
		pd[i].ign2 = current[i].ign2;	
		pd[i].page_table_addr = current[i].page_table_addr;
	}
	for (i = kernel_top_last_dir_entry; i < 1023; i++) {
		pd[i].present = 0;
	}

	// La dernière entrée pointe vers lui même ce qui fait que :
	// 0xFFFFF000 est traduit par la MMU en l'adresse du page directory
	// 0xFFC + 1024 * index_page_table permet d'accéder à une pde
	pd[1023].page_table_addr = pd_paddr >> 12;
	pd[1023].r_w = 1;
	pd[1023].present = 1;
	pd[1023].u_s = 1;
}

/*
 * Chargement d'un page directory
 */
void pagination_load_page_directory(struct page_directory_entry * pd) {
	// On place le rep dans le registre cr3
	asm volatile("mov %0, %%cr3":: "b"(pd));
}

/**
 * Retourne l'emplacement en mémoire virtuelle reservé pour la table de page
 * correspondant à cet identifiant
 */
vaddr_t get_page_table_vaddr(int index_page) {
	return -(1024+index_page)*PAGE_SIZE;
}

