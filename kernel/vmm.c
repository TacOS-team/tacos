#include <memory.h>
#include <pagination.h>
#include <vmm.h>

// Page Table Entry Magic
#define PTE_MAGIC 0xFFC00000


// a slab of pages
// chaque slab commence par un entête (struct slab) et est suivi par les données utiles.
struct slab
{
  struct slab *prev;
  size_t nb_pages;
  struct slab *next;
};

struct slabs_list
{
  struct slab *begin;
  struct slab *end;
};

struct slabs_list free_slabs;
struct slabs_list used_slabs;
vaddr_t vmm_top;

static int is_empty(struct slabs_list *list)
{
  return list->begin == NULL;
}

// Enlève un slab d'une slabs_list
// Le slab doit appartenir à la liste
static void remove(struct slabs_list *list, struct slab *s)
{
  if(s->prev != NULL)
    s->prev->next = s->next;
  if(s->next != NULL)
    s->next->prev = s->prev;

  if(s == list->begin)
    list->begin = s->next;
  if(s == list->end)
    list->end = s->prev;
}

// Ajoute un slab à une slabs_list
// l'ordre de la liste est celui de la mémoire virtuelle
static void add(struct slabs_list *list, struct slab *s)
{
  struct slab *it = list->begin;

  if(is_empty(list))
  {
    list->begin = s;
    s->prev = NULL;
    s->next = NULL;
    list->end = s;
    return;
  }

  while(it->next != NULL && it->next < s)
    it = it->next;

  // add at the end of the list
  if(it->next == NULL)
  {
    list->end->next = s;
    s->prev = list->end;
    s->next = NULL;
    list->end = s;
    return;
  }

  s->prev = it;
  s->next = it->next;
  it->next->prev = s;
  it->next = s;
}

// Retourne l'entrée de répertoire de page correspondant à dir
static struct page_directory_entry *get_pde(int dir)
{
  struct page_directory_entry *page_directory = 
    ((struct page_directory_entry *) 
     ((((struct page_directory_entry *) PTE_MAGIC)->page_table_addr) << 12));
  return page_directory + dir;
}

// Retourne l'entrée de table de page correspondant à dir, table
static struct page_table_entry *get_pte(int dir, int table)
{
  struct page_table_entry *page_table =
    ((struct page_table_entry *) (get_pde(dir)->page_table_addr << 12));
  return page_table + table;
}

// Retourne une adresse lineaire en fonction de sa position dans le rep de page
static vaddr_t get_linear_address(int dir, int table, int offset)
{
  return (((dir&0x3FF) << 22) | ((table&0x3FF) << 12) | (offset&0xFFF));
}

// créer une entrée de page
static void create_page_entry(struct page_table_entry *pte, paddr_t page_addr)
{
  pte->present = 1;
  pte->page_addr = page_addr >> 12;
  pte->r_w = 1;
}

// créer une entrée de répertoire
static void create_page_dir(struct page_directory_entry *pde)
{
  paddr_t page_addr = memory_reserve_page_frame();

  pde->present = 1;
  pde->page_table_addr = page_addr >> 12;
  pde->r_w = 1;

  // XXX : MAP NEW PAGE TO A PTE
}

// Map dans le répertoire des pages une nouvelle page
static void map(paddr_t phys_page_addr, vaddr_t virt_page_addr)
{
  int dir = virt_page_addr >> 22;
  int table = (virt_page_addr & 0x003FF000) >> 12;
  struct page_directory_entry * pde = get_pde(dir);
  struct page_table_entry * pte;

  if (!pde->present)
    create_page_dir(pde);

  pte = get_pte(dir, table);
  create_page_entry(get_pte(dir, table), phys_page_addr);
}

static void unmap(vaddr_t virt_page_addr)
{
  int dir = virt_page_addr >> 22;
  int table = (virt_page_addr & 0x003FF000) >> 12;
  struct page_table_entry *pte = get_pte(dir, table);

  pte->present = 0;
  pte->page_addr = 0;
  pte->r_w = 0;

  // XXX : UNMAP DIR ENTRY IF TABLE IS EMPTY
}

void init_vmm()
{
  paddr_t end_kernel = memory_reserve_page_frame();
  map(end_kernel, end_kernel);

  free_slabs.begin = (struct slab *) end_kernel;
  free_slabs.begin->prev = NULL;
  free_slabs.begin->nb_pages = 1;
  free_slabs.begin->next = NULL;
  free_slabs.end = free_slabs.end;

  used_slabs.begin = NULL;
  used_slabs.end = NULL;

  vmm_top = end_kernel + PAGE_SIZE;
}

static int increase_heap(unsigned int nb_pages)
{
  struct slab *slab = (struct slab *) vmm_top;
  slab->nb_pages = nb_pages;
  add(&free_slabs, slab); // FIXME : implement push_back

  for(; nb_pages > 0 ; nb_pages--)
  {
    map(memory_reserve_page_frame(), vmm_top);
    vmm_top += PAGE_SIZE;
  }

  return 0; // FIXME : erreur en cas de mem full
}

static int is_stuck(struct slab *s1, struct slab *s2)
{
  return (vaddr_t) s1 + s1->nb_pages*sizeof(struct slab) == (vaddr_t) s2;
}

unsigned int allocate_new_page(void **alloc)
{
  return allocate_new_pages(1, alloc);
}

// Alloue nb_pages pages qui sont placé en espace contigüe de la mémoire virtuelle 
unsigned int allocate_new_pages(unsigned int nb_pages, void **alloc)
{
  vaddr_t virt_addr = 0;
  struct slab *slab = free_slabs.begin;

  while(slab != NULL && slab->nb_pages < nb_pages)
    slab = slab->next;

  if(slab == NULL)
  {
    increase_heap(nb_pages);
    slab = free_slabs.end;
  }

  virt_addr = (vaddr_t) slab + sizeof(struct slab);

  // XXX : CUT SLAB
  remove(&free_slabs, slab);
  add(&used_slabs, slab);

  *(alloc) = (void *) virt_addr;
  return nb_pages*PAGE_SIZE - sizeof(struct slab); 
}

// Unallocate a page
void unallocate_page(void *page)
{
  struct slab *slab = used_slabs.begin;

  while(slab != NULL && (vaddr_t) slab + sizeof(struct slab) < (vaddr_t) page)
    slab = slab->next;

  remove(&used_slabs, slab);
  add(&free_slabs, slab);
 
  // Fusionne 2 free_slab qui sont collé
  if(slab->prev != NULL && is_stuck(slab->prev, slab))
  {
    slab->prev->nb_pages += slab->nb_pages;
    remove(&free_slabs, slab);
  }else if(is_stuck(slab, slab->next))
  {
    slab->nb_pages += slab->next->nb_pages;
    remove(&free_slabs, slab->next);
  }

  // Compresse le tas
  if(is_stuck(slab, (struct slab *) vmm_top))
  {
    remove(&free_slabs, slab);
    for(; slab->nb_pages>0 ; slab->nb_pages--)
    {
      vmm_top -= PAGE_SIZE;
      unmap(vmm_top);
    }
  }
}

// taille demandée / taille page + (taille demandee - (taille demandée / taille page) > 0
// retourne le nombre de pages minimal à allouer pour une zone mémoire
// de taille size : entier_sup(size + overhead)
unsigned int calculate_min_pages(size_t size)
{
  double nb_pages = ((double) size + sizeof(struct slab)) / PAGE_SIZE;
  return (unsigned int) (nb_pages + ((nb_pages - (int) nb_pages > 0) ? 1 : 0));
}

