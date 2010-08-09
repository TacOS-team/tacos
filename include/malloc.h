#ifndef _MALLOC_H
#define _MALLOC_H

#include <types.h>

struct virtual_mem;

void init_malloc();
void *malloc(size_t size);
void *__malloc(struct virtual_mem *vm, size_t size);
int free(void *p);
void malloc_print_mem();

#endif

