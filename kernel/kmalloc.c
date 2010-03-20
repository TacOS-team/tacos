#include <types.h>
#include <heap.h>
#include <kmalloc.h>

#define KMALLOC_FREE_MEM_MAX_HEAP_SIZE 256

static size_t *heap[KMALLOC_FREE_MEM_MAX_HEAP_SIZE];

void init_kmalloc()
{
}

