/**
 * @file pagination.h
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

#ifndef _PAGINATION_H_
#define _PAGINATION_H_

#include <types.h>

/**
 * Emplacement de mémoire virtuelle réservé au kernel : 0 -> 1gio
 */
#define _PAGINATION_KERNEL_TOP 0x40000000

extern int pagination_activated;

/**
 * @struct page_directory_entry
 * @brief Page Directory Entry
 *
 * PDE : Référence l'adresse d'une table de pages. (cf doc intel p125)
 */
struct  page_directory_entry {
	uint32_t present:1;				/**< doit être à 1 pour référencer une table de pages */
	uint32_t r_w:1;					/**< 0 writes non autorisés sur la table de page référencée par cette entrée (dépend de CPL et CR0.WP) */
	uint32_t u_s:1;					/**< User/supervisor. Si 0 alors accès avec CPL=3 non autorisés à la table de page référencée par cette entrée. */
	uint32_t pwt:1;					/**< Page-level write-through ?? voir section 4.9 de la doc intel */
	uint32_t pcd:1;					/**< Page-level cache disable, voir section 4.9 de la doc intel */
	uint32_t a:1;						/**< Accessed : indique si un soft a accedé à la table de pages référencée par cette entrée */
	uint32_t ign1:1;					/**< ignoré */
	uint32_t ps:1;						/**< Page size : doit être à 0 sinon c'est qu'on map une page de 4Mio */
	uint32_t ign2:4;					/**< ignoré */
	uint32_t page_table_addr:20;	/**< adresse de la table de page (alignée sur 4kio donc que les 20 bits de poids fort) */
} __attribute__ ((packed));

/**
 * @struct page_table_entry
 * @brief Page Table Entry
 *
 * PTE : Référence l'adresse d'une page.
 */
struct page_table_entry {
	uint32_t present:1;		/**< doit être à 1 pour référencer une page*/
	uint32_t r_w:1;			/**< 0 writes non autorisés sur la page référencée par cette entrée (dépend de CPL et CR0.WP)*/
	uint32_t u_s:1;			/**< User/supervisor. Si 0 alors accès avec CPL=3 non autorisés à la page référencée par cette entrée.*/
	uint32_t pwt:1;			/**< Page-level write-through ?? voir section 4.9 de la doc intel*/
	uint32_t pcd:1;			/**< Page-level cache disable, voir section 4.9 de la doc intel*/
	uint32_t a:1;				/**< Accessed : indique si un soft a accedé à la page référencée par cette entrée*/
	uint32_t d:1;				/**< Dirty : indique si un soft a écrit sur la page*/
	uint32_t pat:1;			/**< si pat activé alors ça détermine le type de mémoire utilisée pour accéder à la page. Sinon faut le mettre à 0.*/
	uint32_t g:1;				/**< ?? cf doc.*/
	uint32_t ignored:3;		/**< ignoré*/
	uint32_t page_addr:20;	/**< adresse de la page (alignée sur 4kio donc que les 20 bits de poids fort)*/
} __attribute__ ((packed));


/**
 * @brief Activation de la pagination et initialisation.
 *
 * Cette fonction s'occupe d'activer la pagination. (identity mapping entre 
 * autre qui permet de pas briser le lien addr phys et addr lineaire au début)
 */
void pagination_setup();

/** 
 * @brief Initialise un repertoire de page.
 * 
 * Initialise un repertoire de page vide.
 *
 * @param pd le repertoire de page à initialiser.
 */
void pagination_init_page_directory(struct page_directory_entry * pd);
void pagination_init_page_directory_copy_kernel_only(struct page_directory_entry *pd, paddr_t pd_paddr); 

void pagination_map(struct page_directory_entry * pagination_kernel, paddr_t page_addr, vaddr_t v_page_addr, int u_s, int r_w);
int pagination_create_page_dir(struct page_directory_entry *pagination_kernel, int index_pd, int u_s);

static inline void pagination_load_page_directory(struct page_directory_entry * pd) __attribute__((always_inline));

/*
 * Chargement d'un page directory
 */
static inline void pagination_load_page_directory(struct page_directory_entry * pd) {
	// On place le rep dans le registre cr3
	asm volatile("mov %0, %%cr3":: "b"(pd));
}

void pagination_load_page_directory(struct page_directory_entry * pd);

vaddr_t get_page_table_vaddr(int index_page); 

#endif
