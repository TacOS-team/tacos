#ifndef _K_MALLOC_H_
#define _K_MALLOC_H_

/**
 * @file kmalloc.h
 */

#include <types.h>

void init_kmalloc();
void *kmalloc(size_t size);
int kfree(void *p);

void *kmalloc_one_aligned_page();

#endif

