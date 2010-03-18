#include <types.h>
#include <heap.h>
#include <kmalloc.h>

#define KMALLOC_FREE_MEM_MAX_HEAP_SIZE 256

static heap_t free_mem;
static size_t heap[KMALLOC_FREE_MEM_MAX_HEAP_SIZE];

static int compare_kmalloc_free_mem(void *p1, void *p2)
{
  return (size_t)p1 - (size_t)p2->size;
}

void init_kmalloc()
{
  initHeap(&free_mem, compare_kmalloc_free_mem, heap,
           sizeof(size_t), KMALLOC_FREE_MEM_MAX_HEAP_SIZE);

  
  
  addElement(&free_mem, page);
}

