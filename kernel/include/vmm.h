#ifndef _VMM_H_
#define _VMM_H_


void *allocate_new_page();
void *allocate_new_pages(unsigned int nb_pages);
int unallocate_page(void *page);

#endif

