/**
 * @file vmm.c
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

#include <memory.h>
#include <pagination.h>
#include <vmm.h>
#include <process.h>
#include <ksyscall.h>
#include <kstdio.h>

/**
 * vmm est l'allocateur qui s'occupe de gérer les pages virtuelles du noyau.
 * Il se base sur l'algorithme first fit (premier emplacement de taille suffisante)
 *
 * Maintien la liste des espaces libres et des espaces occupés (slabs).
 */

// Page Table Entry Magic
#define PTE_MAGIC 0xFFC00000
#define PDE_MAGIC 0xFFFFF000

static int is_empty(struct slabs_list *list)
{
	return list->begin == NULL;
}

// Enlève un slab d'une slabs_list
// Le slab doit appartenir à la liste
static void remove(struct slabs_list *list, struct slab *s)
{
	if(s->prev != NULL)
		s->prev->next = s->next;
	if(s->next != NULL)
		s->next->prev = s->prev;

	if(s == list->begin)
		list->begin = s->next;
	if(s == list->end)
		list->end = s->prev;
}

static void first_element(struct slabs_list *list, struct slab *s)
{
	list->begin = s;
	s->prev = NULL;
	s->next = NULL;
	list->end = s;
}

static void push_back(struct slabs_list *list, struct slab *s)
{
	if(is_empty(list))
		first_element(list, s);
	else
	{
		list->end->next = s;
		s->prev = list->end;
		s->next = NULL;
		list->end = s;
	}
}


// Ajoute un slab à une slabs_list
// l'ordre de la liste est celui de la mémoire virtuelle
static void add(struct slabs_list *list, struct slab *s)
{
	struct slab *it = list->begin;

	if(is_empty(list))
	{
		first_element(list, s);
		return;
	}

	while(it->next != NULL && it->next < s)
		it = it->next;

	// add at the end of the list
	if(it->next == NULL)
	{
		push_back(list, s);
		return;
	}

	s->prev = it;
	s->next = it->next;
	it->next->prev = s;
	it->next = s;
}

// Retourne l'entrée de répertoire de page correspondant à dir
struct page_directory_entry *get_pde(int dir)
{
	struct page_directory_entry *page_directory = 
		(struct page_directory_entry *) PDE_MAGIC;
	return &page_directory[dir];
}

// Retourne l'entrée de table de page correspondant à dir, table
struct page_table_entry *get_pte(int dir, int table)
{
	//  struct page_table_entry *page_table =
	//    ((struct page_table_entry *) (get_pde(dir)->page_table_addr << 12));
	struct page_table_entry *page_table =
		((struct page_table_entry *) PTE_MAGIC) + (1024 * dir);
	return &page_table[table];
}

// Retourne une adresse lineaire en fonction de sa position dans le rep de page
vaddr_t get_linear_address(int dir, int table, int offset)
{
	return (((dir&0x3FF) << 22) | ((table&0x3FF) << 12) | (offset&0xFFF));
}

// créer une entrée de page
static void create_page_entry(struct page_table_entry *pte, paddr_t page_addr)
{
	if (pte->present == 1) 
	{
		kprintf("wtf???? %x\n", page_addr);
	} else 
	{
		pte->present = 1;
		pte->page_addr = page_addr >> 12;
		pte->r_w = 1;
		pte->u_s = 1;
	}
}

// créer une entrée de répertoire
static int create_page_dir(struct page_directory_entry *pde, int dir)
{
	if(memory_get_first_free_page() == NULL)
		return -1;
	paddr_t page_addr = memory_reserve_page_frame();

	pde->present = 1;
	pde->page_table_addr = page_addr >> 12; // check phys or virtual
	pde->r_w = 1;
	pde->u_s = 1;

	// map new PTE
	vaddr_t v_page_addr = get_page_table_vaddr(dir);
	map(page_addr, v_page_addr);

	int i;
	struct page_table_entry *pte = (struct page_table_entry *) v_page_addr;
	for(i=0 ; i < 1024 ; i++)
		(pte + i)->present = 0;

	return 0;
}

/* XXX : A changer de fichier (pagination l'utilise au final) */
// Map dans le répertoire des pages une nouvelle page
int map(paddr_t phys_page_addr, vaddr_t virt_page_addr)
{
	int dir = virt_page_addr >> 22;
	int table = (virt_page_addr >> 12) & 0x3FF;
	struct page_directory_entry * pde = get_pde(dir);

	if (!pde->present && create_page_dir(pde, dir) == -1)
		return -1;

	create_page_entry(get_pte(dir, table), phys_page_addr);

	return 0;
}

static void unmap(vaddr_t virt_page_addr)
{
	int dir = virt_page_addr >> 22;
	int table = (virt_page_addr & 0x003FF000) >> 12;
	struct page_table_entry *pte = get_pte(dir, table);

	pte->present = 0;
	pte->page_addr = 0;
	pte->r_w = 0;

	// XXX : UNMAP DIR ENTRY IF TABLE IS EMPTY (necessary ?)
}

void init_vmm(struct virtual_mem *kvm)
{
	vaddr_t page_sup_end_kernel = memory_get_kernel_top();

	map(memory_reserve_page_frame(), page_sup_end_kernel);

	kvm->free_slabs.begin = (struct slab *) page_sup_end_kernel; 
	kvm->free_slabs.begin->prev = NULL;
	kvm->free_slabs.begin->nb_pages = 1;
	kvm->free_slabs.begin->next = NULL;
	kvm->free_slabs.end = kvm->free_slabs.begin;

	kvm->used_slabs.begin = NULL;
	kvm->used_slabs.end = NULL;

	kvm->vmm_top = page_sup_end_kernel + PAGE_SIZE;
}

void init_process_vm(struct virtual_mem *vm, int init_nb_pages) 
{
  int i;

	/* laissez une page libre, pour détecter page fault avec un pointeur à null */
	vaddr_t vm_begin = _PAGINATION_KERNEL_TOP;
	
	for(i = 0; i < (init_nb_pages+1); i++)
		map(memory_reserve_page_frame(), vm_begin + i*PAGE_SIZE);

	//vm->used_slabs.begin = (struct slab *) vm_begin;
	vm->used_slabs.begin = (struct slab *) (vm_begin + init_nb_pages*PAGE_SIZE - sizeof(struct slab));
	vm->used_slabs.begin->prev = NULL;
	vm->used_slabs.begin->nb_pages = init_nb_pages;
	vm->used_slabs.begin->next = NULL;
	vm->used_slabs.end = vm->used_slabs.begin;

	vm->free_slabs.begin = (struct slab *) (vm_begin + init_nb_pages*PAGE_SIZE);
	vm->free_slabs.begin->prev = NULL;
	vm->free_slabs.begin->nb_pages = 1;
	vm->free_slabs.begin->next = NULL;
	vm->free_slabs.end = vm->free_slabs.begin;

	vm->vmm_top = vm_begin + (init_nb_pages+1)*PAGE_SIZE;
}

// Agrandit le heap de nb_pages pages et ajoute le nouveau slab à la fin de free_pages
static int increase_heap(struct virtual_mem *vm, unsigned int nb_pages)
{
	unsigned int i;
	/* vaddr_t top_last_slab; unused */
	struct slab *slab = (struct slab *) vm->vmm_top;

	for(i=0 ; i<nb_pages ; i++)
	{
		if(memory_get_first_free_page() == NULL)
			return -1;
		if(map(memory_reserve_page_frame(), vm->vmm_top) == -1)
			return -1;
		vm->vmm_top += PAGE_SIZE;
	}
	
	/* Il semble que si on calcul cette ligne à cet endroit, et que on a !is_empty(&(vm->free_slabs)), ça plante, alors
	 * je commente et j'insert directement la ligne dans le if 
	 * top_last_slab = (vaddr_t) vm->free_slabs.end + vm->free_slabs.end->nb_pages*PAGE_SIZE;*/
	
	if(!is_empty(&(vm->free_slabs)) && ((vaddr_t) vm->free_slabs.end + vm->free_slabs.end->nb_pages*PAGE_SIZE) == vm->vmm_top)
	{
	 	// agrandit juste le dernier free slab
		vm->free_slabs.end->nb_pages += nb_pages;
	}else
	{
		slab->nb_pages = nb_pages;
		push_back(&(vm->free_slabs), slab);
	}

	return 0;
}

static int is_stuck(struct slab *s1, struct slab *s2)
{
	return s1 != NULL &&
		(vaddr_t) s1 + s1->nb_pages*sizeof(struct slab) == (vaddr_t) s2;
}

// Sépare un slab en deux quand il est trop grand et déplace le slab de free_pages vers used_pages
static void cut_slab(struct virtual_mem *vm, struct slab *s, 
										 unsigned int nb_pages)
{
	if(s->nb_pages > nb_pages) // cut slab
	{
		struct slab *new_slab =
			(struct slab*) ((vaddr_t) s + nb_pages*PAGE_SIZE);
		new_slab->nb_pages = s->nb_pages - nb_pages;
		new_slab->prev = s->prev;
		new_slab->next = s->next;

		if(s->next != NULL)
			s->next->prev = new_slab;
		if(s->prev != NULL)
			s->prev->next = new_slab;

		s->nb_pages = nb_pages;
	} else {
	remove(&(vm->free_slabs), s);
	}

	add(&(vm->used_slabs), s);
}

// Alloue une page
unsigned int allocate_new_page(struct virtual_mem *vm, void **alloc)
{
	return allocate_new_pages(vm, 1, alloc);
}

// Alloue nb_pages pages qui sont placé en espace contigüe de la mémoire virtuelle 
unsigned int allocate_new_pages(struct virtual_mem *vm, unsigned int nb_pages,
																void **alloc)
{
	struct slab *slab = vm->free_slabs.begin;

	while(slab != NULL && slab->nb_pages < nb_pages)
		slab = slab->next;

	if(slab == NULL)
	{
		if(increase_heap(vm, nb_pages) == -1)
			return 0;

		slab = vm->free_slabs.end;
	}

	cut_slab(vm, slab, nb_pages);

	*(alloc) = (void *) ((vaddr_t) slab + sizeof(struct slab));
	return nb_pages*PAGE_SIZE - sizeof(struct slab); 
}

// Unallocate a page
int unallocate_page(struct virtual_mem *vm, void *page)
{
	struct slab *slab = vm->used_slabs.begin;

	while(slab != NULL && (vaddr_t) slab + sizeof(struct slab) < (vaddr_t) page)
		slab = slab->next;

	if(slab == NULL)
		return -1;

	remove(&(vm->used_slabs), slab);
	add(&(vm->free_slabs), slab);

	// Fusionne 2 free_slab qui sont collé
	if(is_stuck(slab->prev, slab))
	{
		slab->prev->nb_pages += slab->nb_pages;
		remove(&(vm->free_slabs), slab);

		slab = slab->prev;
	}

	if(is_stuck(slab, slab->next))
	{
		slab->nb_pages += slab->next->nb_pages;
		remove(&(vm->free_slabs), slab->next);
	}

	// Compresse le tas
	if(is_stuck(slab, (struct slab *) vm->vmm_top))
	{
		remove(&(vm->free_slabs), slab);
		for(; slab->nb_pages>0 ; slab->nb_pages--)
		{
			vm->vmm_top -= PAGE_SIZE;
			unmap(vm->vmm_top);
		}
	}

	return 0;
}

static void print_slab(struct slab* s, bool free)
{
	// set_attribute(BLACK, free ? GREEN : RED);
	if(free)
		kprintf("\033[31m");
	else
		kprintf("\033[34m");

	kprintf("[%x ; %x ; %x] ", s, s->prev, s->next);
	kprintf("\033[0m");
}

void vmm_print_heap(struct virtual_mem *vm)
{
	struct slab *free_it = vm->free_slabs.begin;
	struct slab *used_it = vm->used_slabs.begin;

	kprintf("\n-- VMM : printing heap --\n");

	while(free_it != NULL && used_it != NULL)
	{
		if(free_it < used_it)
		{
			print_slab(free_it, TRUE);
			free_it = free_it->next;
		} else
		{
			print_slab(used_it, FALSE);
			used_it = used_it->next;
		}
	}

	while(free_it != NULL)
	{
		print_slab(free_it, TRUE);
		free_it = free_it->next;
	}

	while(used_it != NULL)
	{
		print_slab(used_it, FALSE);
		used_it = used_it->next;
	}

	kprintf("\nVMM TOP : %x\n", vm->vmm_top);

	kprintf("-- VMM : end --\n");
}

void sys_vmm(uint32_t vm, uint32_t nb_pages, uint32_t args) {
	void **alloc = (void *) (((uint32_t *) args)[0]);
	size_t *real_alloc_size = (size_t *) (((uint32_t *) args)[1]);

	asm("cli");
	*real_alloc_size = allocate_new_pages((struct virtual_mem *) vm, nb_pages, 
																				alloc); 
	asm("sti");
}

paddr_t vmm_get_page_paddr(vaddr_t vaddr) {
	int dir = vaddr >> 22;
	int table = (vaddr >> 12) & 0x3FF;
	struct page_table_entry *pte = get_pte(dir, table);

	return pte->page_addr << 12;
}

