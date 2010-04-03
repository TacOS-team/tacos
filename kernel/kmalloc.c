#include <types.h>
#include <stdio.h>
#include <heap.h>
#include <vmm.h>
#include <kmalloc.h>

struct mem
{
  struct mem *prev;
  size_t size; // (struct mem) + data
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

static void add_first_element(struct mem_list *list, struct mem *m)
{
  list->begin = m;
  m->prev = NULL;
  m->next = NULL;
  list->end = m;
}

static void push_back(struct mem_list *list, struct mem *m)
{
  if(is_empty(list))
    add_first_element(list, m);
  else
  {
    list->end->next = m;
    m->prev = list->end;
    m->next = NULL;
    list->end = m;
  }
}

// Ajoute une mem à une mem_list
// l'ordre de la liste est celui de la mémoire virtuelle
static void add(struct mem_list *list, struct mem *m)
{
  struct mem *it = list->begin;

  if(is_empty(list))
  {
    add_first_element(list, m);
    return;
  }

  while(it->next != NULL && it->next < m)
    it = it->next;

  // add at the end of the list
  if(it->next == NULL)
  {
    push_back(list, m);
    return;
  }

  m->prev = it;
  m->next = it->next;
  it->next->prev = m;
  it->next = m;
}

// Sépare un bloc mem en deux quand il est trop grand et déplace le bloc de free_mem vers used_mem
static void cut_mem(struct mem* m, size_t size)
{
  if(m->size > size) // cut mem
  {
    struct mem *new_mem = (struct mem*) ((vaddr_t) m + size);
    new_mem->size = m->size - size;
    add(&free_mem, new_mem);

    m->size = size;
  } 

  remove(&free_mem, m);
  add(&allocated_mem, m);
}

static int is_stuck(struct mem *m1, struct mem *m2)
{
  return m1 != NULL && (vaddr_t) m1 + m1->size == (vaddr_t) m2;
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
    
    if(real_alloc_size <= 0)
      return NULL;
    
    new_mem = (struct mem *) alloc;
    new_mem->size = real_alloc_size;
    push_back(&free_mem, new_mem);
    mem = free_mem.end;
  }

  cut_mem(mem, real_size);
  //asm("sti");
  return (void *) (((uint32_t) mem) + sizeof(struct mem));
}

int kfree(void *p)
{
  struct mem *m = allocated_mem.end;

  //asm("cli");
  
  while(m != NULL) {
    if((vaddr_t) m + sizeof(struct mem) <= (vaddr_t) p &&
       (vaddr_t) p < (vaddr_t) m + m->size)
      break;
    m = m->prev;
  }

  if(m == NULL)
    return -1;

  remove(&allocated_mem, m);
  add(&free_mem, m);

  if(is_stuck(m->prev, m))
  {
    m->prev->size += m->size;
    remove(&free_mem, m);
    m = m->prev;
  }
  
  if(is_stuck(m, m->next))
  {
    m->size +=  m->next->size;
    remove(&free_mem, m->next);
  }

  //asm("sti");
  return 0;
}

static void print_mem(struct mem* m, bool free)
{
  set_attribute(BLACK, free ? GREEN : RED);
  printf("[%x ; %d ; %x ; %x] ", m, m->size,  m->prev, m->next);
  fflush(stdout);
  reset_attribute();
}

void kmalloc_print_mem()
{
  struct mem *free_it = free_mem.begin;
  struct mem *allocated_it = allocated_mem.begin;

  vmm_print_heap();

  printf("\n-- kmalloc : printing heap --\n");

  while(free_it != NULL && allocated_it != NULL)
  {
    if(free_it < allocated_it)
    {
      print_mem(free_it, TRUE);
      free_it = free_it->next;
    } else
    {
      print_mem(allocated_it, FALSE);
      allocated_it = allocated_it->next;
    }
  }

  while(free_it != NULL)
  {
    print_mem(free_it, TRUE);
    free_it = free_it->next;
  }

  while(allocated_it != NULL)
  {
    print_mem(allocated_it, FALSE);
    allocated_it = allocated_it->next;
  }
  
  printf("\n-- kmalloc : end --\n\n\n");
}

