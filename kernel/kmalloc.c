/**
 * @file kmalloc.c
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

#include <types.h>
#include <malloc.h>
#include <memory.h>
#include <vmm.h>
#include <kmalloc.h>

static struct virtual_mem kvm;
static struct mem_list k_free_mem;
static struct mem_list k_allocated_mem;

void init_kmalloc()
{
	init_vmm(&kvm);
	init_malloc(&k_free_mem, &k_allocated_mem);
}

void *kmalloc(size_t size)
{ 
  return __malloc(&kvm, &k_free_mem, &k_allocated_mem, size); 
}

int kfree(void *p)
{
	return __free(p, &k_free_mem, &k_allocated_mem);
}

/* XXX ÜBER MOCHE, mais au moins ça doit marcher... */
void *kmalloc_one_aligned_page() {
	uint32_t tmp;
	allocate_new_pages(&kvm, 2, (void **) &tmp);
	
	return (void *) memory_align_page_sup((paddr_t) tmp);
}

