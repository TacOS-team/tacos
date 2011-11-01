/**
 * @file malloc.h
 *
 * @author TacOS developers 
 *
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

#ifndef _MALLOC_H
#define _MALLOC_H

#include <sys/types.h>

struct mem_list
{
  struct mem *begin;
  struct mem *end;
};

struct virtual_mem;

void init_malloc(struct mem_list *free_mem, struct mem_list *allocated_mem);
void init_process_malloc();
void *malloc(size_t size);
void *__malloc(struct virtual_mem *vm, struct mem_list *free_mem, 
							 struct mem_list *allocated_mem, size_t size);
int free(void *p);
int __free(void *p, struct mem_list *free_mem, struct mem_list *allocated_mem);
void malloc_print_mem(struct mem_list *free_mem, 
											struct mem_list *allocated_mem);

#endif

