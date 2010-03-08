#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <types.h>

typedef struct malloc_page_
{
  uint16_t nbAllocated;
  struct malloc_page_ *next;
} malloc_page;

typedef struct
{
  malloc_page *begin;
  malloc_page *end;
} malloc_page_queue;

typedef struct mallocated_ 
{
  malloc_page *page;
  paddr_t addr; 
  size_t size;

  struct mallocated_ *next;
  struct mallocated_ *prev;
} mallocated;

typedef struct
{
  mallocated *begin;
  mallocated *end;
} mallocated_queue;

static malloc_page_queue pages = {NULL, NULL};
static mallocated_queue allocatedMem = {NULL, NULL};

// Quand on réserve une page, les sizeof(malloc_page) premiers octets sont
// réserver pour pour la description de l'occutpion de cette page.
malloc_page *reserve_page()
{
  malloc_page *p = (malloc_page *) memory_reserve_page_frame();
  p->nbAllocated = 0;
  p->next = NULL;

  return p;
}

// On réserve une première page pour enregistrer toutes les infos sur les
// allocations mémoire
void *init_malloc(size_t size)
{
  mallocated *newAlloc;
  pages.begin = reserve_page();
  pages.end = pages.begin;

  newAlloc = (mallocated *) (pages.end + 1);
  newAlloc->page = pages.end;
 // XXX : collision stack/mem
  newAlloc->addr = ((paddr_t) pages.end) + PAGE_SIZE - size;
  newAlloc->size = size;
  newAlloc->next = NULL;
  newAlloc->prev = NULL;

  allocatedMem.begin = newAlloc;
  allocatedMem.end = newAlloc;
  pages.end->nbAllocated++;

  return (void *) newAlloc->addr;
}

void *malloc(size_t size)
{
  mallocated *newAlloc;

  if(pages.begin == NULL)
    return init_malloc(size);

  newAlloc = allocatedMem.end + 1;
  newAlloc->page = allocatedMem.end->page;
  newAlloc->addr = allocatedMem.end->addr - size;
  newAlloc->size = size;
  newAlloc->next = NULL;
  newAlloc->prev = allocatedMem.end;
  allocatedMem.end->next = newAlloc;
  
  if(allocatedMem.begin == NULL)
    allocatedMem.begin = newAlloc;
  allocatedMem.end = newAlloc;

  pages.end->nbAllocated++;

  return (void *) newAlloc->addr;
}

void free(void *addr)
{
  mallocated *m = allocatedMem.begin;

  while(m != NULL && m->addr != (paddr_t) addr)
    m = m->next;

  if(m != NULL)
  {
    m->page->nbAllocated--;
    if(m->prev == NULL)
    {
      allocatedMem.begin = m->next;
      m->next->prev = NULL;
    } else
    {
      m->prev->next = m->next;
    }

    if(m->next == NULL)
    {
      allocatedMem.end = m->prev;
      m->prev->next = NULL;
    } else
    {
      m->next->prev = m->prev;
    }
  }
}

void printMallocated()
{
  mallocated *m = allocatedMem.begin;

  printf("Mallocated %d : ", pages.end->nbAllocated);
  while(m != NULL)
  {
    printf("[%d, %d] ", m->addr, m->size);
    m = m->next;
  }
  printf("\n");
}

