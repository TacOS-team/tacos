#ifndef _K_MALLOC_H
#define _K_MALLOC_H

void init_kmalloc();
void *kmalloc(size_t size);
void kfree(void *p);

#endif

