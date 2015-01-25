/**
 * @file vmm.c
 *
 * @author TacOS developers 
 *
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
 * vmm est l'allocateur qui s'occupe de gérer les pages virtuelles du noyau.
 * Il se base sur l'algorithme first fit (premier emplacement de taille suffisante)
 *
 * Maintien la liste des espaces libres et des espaces occupés (slabs).
 */

#include <memory.h>
#include <pagination.h>
#include <vmm.h>
#include <ksyscall.h>
#include <kstdio.h>
#include <scheduler.h>

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


// Map dans le répertoire des pages une nouvelle page
int map(paddr_t phys_page_addr, vaddr_t virt_page_addr, int u_s, int r_w)
{
	pagination_map((struct page_directory_entry *) PDE_MAGIC, phys_page_addr, virt_page_addr, u_s, r_w);

	return 0;
}

void unmap(vaddr_t virt_page_addr)
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

	map(reserve_page_frame(NULL), page_sup_end_kernel, 0, 1);

	kvm->free_slabs.begin = (struct slab *) page_sup_end_kernel;
	kvm->free_slabs.begin->prev = NULL;
	kvm->free_slabs.begin->nb_pages = 1;
	kvm->free_slabs.begin->next = NULL;
	kvm->free_slabs.end = kvm->free_slabs.begin;

	kvm->used_slabs.begin = NULL;
	kvm->used_slabs.end = NULL;

	kvm->vmm_top = page_sup_end_kernel + PAGE_SIZE;
}

void init_process_vm(process_t *process, struct virtual_mem *vm, int init_nb_pages)
{
	int i;

	vaddr_t vm_begin = _PAGINATION_KERNEL_TOP;
	
	for(i = 0; i < (init_nb_pages+1); i++)
		map(reserve_page_frame(process), vm_begin + i*PAGE_SIZE, 1, 1); //XXX: u_s = 0 ?

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
static int increase_heap(process_t *process, struct virtual_mem *vm, unsigned int nb_pages, int u_s)
{
	unsigned int i;
	/* vaddr_t top_last_slab; unused */
	struct slab *slab = (struct slab *) vm->vmm_top;

	for (i = 0; i < nb_pages; i++){
		paddr_t paddr = reserve_page_frame(process);
		if(!paddr || map(paddr, vm->vmm_top, u_s, 1) == -1)
			return -1;
		vm->vmm_top += PAGE_SIZE;
	}
	
	// S'il y a un free slab et que la fin correspond à la fin du heap:
	if(!is_empty(&(vm->free_slabs)) && ((vaddr_t) vm->free_slabs.end + vm->free_slabs.end->nb_pages*PAGE_SIZE) == (vaddr_t)slab)
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
static struct slab * cut_slab(struct virtual_mem *vm, struct slab *s, unsigned int nb_pages)
{
	if(s->nb_pages > nb_pages) // cut slab
	{
		// Fin du slab utilisé pour l'allocation demandée.
		struct slab *new_used_slab = (struct slab*) ((vaddr_t) s + (s->nb_pages - nb_pages)*PAGE_SIZE);
		new_used_slab->nb_pages = nb_pages;
		add(&(vm->used_slabs), new_used_slab);

		// Actualisation du nombre de pages du slab coupé.
		s->nb_pages -= nb_pages;

		// On renvoie le morceau de slab utilisé.
		return new_used_slab;
	} else {
		// On utilise entièrement le slab donc on retire des slabs free pour le passer en used.
		remove(&(vm->free_slabs), s);
		add(&(vm->used_slabs), s);

		// Et on renvoie le slab.
		return s;
	}
}

// Alloue une page
unsigned int allocate_new_page(process_t *process, struct virtual_mem *vm, void **alloc, int u_s)
{
	return allocate_new_pages(process, vm, 1, alloc, u_s);
}

// Alloue nb_pages pages qui sont placé en espace contigüe de la mémoire virtuelle
unsigned int allocate_new_pages(process_t *process, struct virtual_mem *vm, unsigned int nb_pages, void **alloc, int u_s)
{
	struct slab *slab = vm->free_slabs.begin;

	while(slab != NULL && slab->nb_pages < nb_pages)
		slab = slab->next;

	if(slab == NULL)
	{
		if(increase_heap(process, vm, nb_pages, u_s) == -1)
			return 0;

		slab = vm->free_slabs.end;
	}

	*(alloc) = (void *) ((vaddr_t) cut_slab(vm, slab, nb_pages) + sizeof(struct slab));
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
			print_slab(free_it, true);
			free_it = free_it->next;
		} else
		{
			print_slab(used_it, false);
			used_it = used_it->next;
		}
	}

	while(free_it != NULL)
	{
		print_slab(free_it, true);
		free_it = free_it->next;
	}

	while(used_it != NULL)
	{
		print_slab(used_it, false);
		used_it = used_it->next;
	}

	kprintf("\nVMM TOP : %x\n", vm->vmm_top);

	kprintf("-- VMM : end --\n");
}

void sys_vmm(uint32_t min_size, void **alloc, size_t *real_alloc_size) {
	process_t *process = get_current_process();
	struct virtual_mem* vm = process->vm;

	asm("cli");
	*real_alloc_size = allocate_new_pages(process, vm, calculate_min_pages(min_size),
                                          alloc, 1);
	asm("sti");
}

paddr_t vmm_get_page_paddr(vaddr_t vaddr) {
	int dir = vaddr >> 22;
	int table = (vaddr >> 12) & 0x3FF;
	struct page_table_entry *pte = get_pte(dir, table);

	return pte->page_addr << 12;
}

unsigned int calculate_min_pages(size_t size)
{
	unsigned int total_size = size + sizeof(struct slab);
	return total_size / PAGE_SIZE + (total_size % PAGE_SIZE > 0);
}

