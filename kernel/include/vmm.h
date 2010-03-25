#ifndef _VMM_H_
#define _VMM_H_


void init_vmm();
unsigned int allocate_new_page(void **alloc);
unsigned int allocate_new_pages(unsigned int nb_pages, void **alloc);
int unallocate_page(void *page);
unsigned int calculate_min_pages(size_t size);

#endif

