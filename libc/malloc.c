/**
 * @file malloc.c
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

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <malloc.h>

struct mem_list
{
  struct mem *begin;
  struct mem *end;
};

struct mem
{
  struct mem *prev;
  size_t size; // (struct mem) + data
  struct mem *next;
};

static struct mem_list process_free_mem;
static struct mem_list process_allocated_mem;

static void *__malloc(struct mem_list *free_mem, struct mem_list *allocated_mem,
                      size_t size);
static int __free(void *p, struct mem_list *free_mem,
                  struct mem_list *allocated_mem);

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

static void add_first_element(struct mem_list *list, struct mem *m)
{
  list->begin = m;
  m->prev = NULL;
  m->next = NULL;
  list->end = m;
}

static void push_back(struct mem_list *list, struct mem *m)
{
  if(is_empty(list))
    add_first_element(list, m);
  else
  {
    list->end->next = m;
    m->prev = list->end;
    m->next = NULL;
    list->end = m;
  }
}

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
    push_back(list, m);
    return;
  }

  m->prev = it;
  m->next = it->next;
  it->next->prev = m;
  it->next = m;
}

// Sépare un bloc mem en deux quand il est trop grand et déplace le bloc de free_mem vers used_mem
static void cut_mem(struct mem* m, size_t size, 
										struct mem_list *free_mem, struct mem_list *allocated_mem)
{
  if(m->size - size > sizeof(struct mem)) // cut mem
  {
    struct mem *new_mem = (struct mem*) ((vaddr_t) m + size);
    new_mem->size = m->size - size;
    add(free_mem, new_mem);

    m->size = size;
  } 

  remove(free_mem, m);
  add(allocated_mem, m);
}

static int is_stuck(struct mem *m1, struct mem *m2)
{
  return m1 != NULL && (vaddr_t) m1 + m1->size == (vaddr_t) m2;
}

void init_process_malloc()
{
	process_free_mem.begin = NULL;
	process_free_mem.end = NULL;
	process_allocated_mem.begin = NULL;
	process_allocated_mem.end = NULL;
}

void *malloc(size_t size) {
	return __malloc(&process_free_mem, &process_allocated_mem, size);
}

void *__malloc(struct mem_list *free_mem, struct mem_list *allocated_mem,
               size_t size)
{ 
  struct mem *mem = free_mem->begin;
  size_t real_size = size + sizeof(struct mem);

  while(mem != NULL && mem->size < real_size)
    mem = mem->next;

  if(mem == NULL)
  {
    struct mem *new_mem;
    struct mem *alloc;
    size_t real_alloc_size;

		syscall(SYS_VMM, (uint32_t) real_size, (uint32_t) &alloc,
                     (uint32_t) &real_alloc_size);
    
    if(real_alloc_size <= 0)
      return NULL;
    
    new_mem = (struct mem *) alloc;
    new_mem->size = real_alloc_size;
    push_back(free_mem, new_mem);
    mem = free_mem->end;
  }

  cut_mem(mem, real_size, free_mem, allocated_mem);
  return (void *) (((uint32_t) mem) + sizeof(struct mem));
}

int free(void *p) {
	return __free(p, &process_free_mem, &process_allocated_mem);
}

int __free(void *p, struct mem_list *free_mem, struct mem_list *allocated_mem)
{
  struct mem *m = allocated_mem->end;

  while(m != NULL) {
    if((vaddr_t) m + sizeof(struct mem) <= (vaddr_t) p &&
       (vaddr_t) p < (vaddr_t) m + m->size)
      break;
    m = m->prev;
  }

  if(m == NULL)
    return -1;

  remove(allocated_mem, m);
  add(free_mem, m);

  if(is_stuck(m->prev, m))
  {
    m->prev->size += m->size;
    remove(free_mem, m);
    m = m->prev;
  }
  
  if(is_stuck(m, m->next))
  {
    m->size += m->next->size;
    remove(free_mem, m->next);
  }

  return 0;
}

static void print_mem(struct mem* m, bool free __attribute__ ((unused)))
{
 // set_attribute(BLACK, free ? GREEN : RED);
  printf("[%x ; %d ; %x ; %x] ", m, m->size,  m->prev, m->next);
  fflush(stdout);
//  reset_attribute();
}

void malloc_print_mem()
{
  struct mem *free_it = process_free_mem.begin;
  struct mem *allocated_it = process_allocated_mem.begin;

  printf("\n-- malloc : printing heap --\n");

	/*int i = 0;*/
  while(free_it != NULL && allocated_it != NULL)
  {
    if(free_it < allocated_it)
    {
      print_mem(free_it, true);
      free_it = free_it->next;
    } else
    {
      print_mem(allocated_it, false);
      allocated_it = allocated_it->next;
    }
  }

  while(free_it != NULL)
  {
    print_mem(free_it, true);
    free_it = free_it->next;
  }

  while(allocated_it != NULL)
  {
    print_mem(allocated_it, false);
    allocated_it = allocated_it->next;
  }
  
  printf("\n-- malloc : end --\n\n\n");
}

