#include <types.h>
#include <heap.h>
#include <vmm.h>
#include <kmalloc.h>

struct mem
{
  struct mem *prev;
  size_t size; // possible data
  struct mem *next;
};

struct mem_list
{
  struct mem *begin;
  struct mem *end;
};

struct mem_list free_mem;
struct mem_list allocated_mem;

static int is_empty(struct mem_list *list)
{
  return list->begin == NULL;
}

// Enlève une mem d'une mem_list
// Le bloc doit appartenir à la liste
static void remove(struct mem_list *list, struct mem *m)
{
  if(m->prev != NULL)
    m->prev->next = m->next;
  if(m->next != NULL)
    m->next->prev = m->prev;

  if(m == list->begin)
    list->begin = m->next;
  if(m == list->end)
    list->end = m->prev;
}

// Ajoute une mem à une mem_list
// l'ordre de la liste est celui de la mémoire virtuelle
static void add(struct mem_list *list, struct mem *m)
{
  struct mem *it = list->begin;

  if(is_empty(list))
  {
    list->begin = m;
    m->prev = NULL;
    m->next = NULL;
    list->end = m;
    return;
  }

  while(it->next != NULL && it->next < m)
    it = it->next;

  // add at the end of the list
  if(it->next == NULL)
  {
    list->end->next = m;
    m->prev = list->end;
    m->next = NULL;
    list->end = m;
    return;
  }

  m->prev = it;
  m->next = it->next;
  it->next->prev = m;
  it->next = m;
}

void init_kmalloc()
{
  free_mem.begin = NULL;
  free_mem.end = NULL;
  allocated_mem.begin = NULL;
  allocated_mem.end = NULL;
}

void *kmalloc(size_t size)
{ 
  struct mem *mem = free_mem.begin;
  size_t real_size = size + sizeof(struct mem);

  //asm("cli"); // sinon on risque d'avoir des trucs bizarre

  while(mem != NULL && mem->size < real_size)
    mem = mem->next;

  if(mem == NULL)
  {
    struct mem *new_mem;
    void *alloc;
    size_t real_alloc_size = allocate_new_pages(calculate_min_pages(real_size), 
                                                &alloc);
    new_mem = (struct mem *) alloc;
    new_mem->size = real_alloc_size;
    add(&free_mem, new_mem); // XXX : push_back
    mem = free_mem.end;
  }

  // XXX : CUT MEM
  remove(&free_mem, mem);
  add(&allocated_mem, mem);
  
  asm("sti");

  return (void *) (((uint32_t) mem) + sizeof(struct mem));
}

void kfree(void *p)
{
  asm("cli"); 
  asm("sti");
}

