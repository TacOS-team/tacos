#include <malloc.h>
#include <mempage.h>

// XXX : a refaire xD

typedef struct mallocated_ 
{
  malloc_page *page;
  uint16_t addr; // inner page address
  size_t size;

  mallocated_ *next;
} mallocated;

typedef struct malloc_page_
{
  uint16_t nbAllocated;
  malloc_page_ *next;
} malloc_page;

static malloc_page *pages = NULL;
static mallocated *allocatedMem = NULL;

malloc_page reserve_page()
{
  malloc_page *p = mempage_reserve_page();
  p->nbAllocated = 0;
  p->next = NULL;
  
  return p;
}

void *malloc(size_t n, size_t size)
{
  if(pages == NULL)
    pages = reserve_page();

}

