#ifndef _MALLOC_H
#define _MALLOC_H

#include <types.h>

struct mem_list
{
  struct mem *begin;
  struct mem *end;
};

struct virtual_mem;

void init_malloc(struct mem_list *free_mem, struct mem_list *allocated_mem);
void *malloc(size_t size);
void *__malloc(struct virtual_mem *vm, struct mem_list *free_mem, 
							 struct mem_list *allocated_mem, size_t size);
int free(void *p);
int __free(void *p, struct mem_list *free_mem, struct mem_list *allocated_mem);
void malloc_print_mem(struct mem_list *free_mem, 
											struct mem_list *allocated_mem);

#endif

