#ifndef _K_MALLOC_H
#define _K_MALLOC_H

void init_kmalloc();
void *kmalloc(size_t size);
int kfree(void *p);

#endif

