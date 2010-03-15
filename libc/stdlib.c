#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <types.h>
#include <pagination.h>
#include <heap.h>

heap_t *heap = NULL;
//void initHeap(heap_t* h, cmp_func_type cmp);

int init_malloc()
{
	struct page_directory_entry * pagination_kernel;

  heap = (heap_t *) memory_reserve_page_frame(); 

  //initHeap(heap, ); 
}

void *malloc(size_t size)
{
  if(heap == NULL && init_malloc() == -1)
    return -1;
}

void free(void *addr)
{

}

