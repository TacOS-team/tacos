#include <memory.h>
#include <pagination.h>
#include <vmm.h>
#include <stdio.h>
// TODO : dégager cette dépendance.
#include <video.h>

/**
 * vmm est l'allocateur qui s'occupe de gérer les pages virtuelles du noyau.
 * Il se base sur l'algorithme first fit (premier emplacement de taille suffisante)
 *
 * Maintien la liste des espaces libres et des espaces occupés (slabs).
 */

// Page Table Entry Magic
#define PTE_MAGIC 0xFFC00000
#define PDE_MAGIC 0xFFFFF000

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

// Prototypes
static int map(paddr_t phys_page_addr, vaddr_t virt_page_addr);

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

static void first_element(struct slabs_list *list, struct slab *s)
{
  list->begin = s;
  s->prev = NULL;
  s->next = NULL;
  list->end = s;
}

static void push_back(struct slabs_list *list, struct slab *s)
{
  if(is_empty(list))
    first_element(list, s);
  else
  {
    list->end->next = s;
    s->prev = list->end;
    s->next = NULL;
    list->end = s;
  }
}


// Ajoute un slab à une slabs_list
// l'ordre de la liste est celui de la mémoire virtuelle
static void add(struct slabs_list *list, struct slab *s)
{
  struct slab *it = list->begin;

  if(is_empty(list))
  {
    first_element(list, s);
    return;
  }

  while(it->next != NULL && it->next < s)
    it = it->next;

  // add at the end of the list
  if(it->next == NULL)
  {
    push_back(list, s);
    return;
  }

  s->prev = it;
  s->next = it->next;
  it->next->prev = s;
  it->next = s;
}

// Retourne l'entrée de répertoire de page correspondant à dir
struct page_directory_entry *get_pde(int dir)
{
  struct page_directory_entry *page_directory = 
	  (struct page_directory_entry *) PDE_MAGIC;
  return &page_directory[dir];
}

// Retourne l'entrée de table de page correspondant à dir, table
struct page_table_entry *get_pte(int dir, int table)
{
//  struct page_table_entry *page_table =
//    ((struct page_table_entry *) (get_pde(dir)->page_table_addr << 12));
  struct page_table_entry *page_table =
     ((struct page_table_entry *) PTE_MAGIC) + (1024 * dir);
  return &page_table[table];
}

// Retourne une adresse lineaire en fonction de sa position dans le rep de page
vaddr_t get_linear_address(int dir, int table, int offset)
{
  return (((dir&0x3FF) << 22) | ((table&0x3FF) << 12) | (offset&0xFFF));
}

// créer une entrée de page
static void create_page_entry(struct page_table_entry *pte, paddr_t page_addr)
{
	if (pte->present == 1) {
		kprintf("wtf????\n");
	} else {
		pte->present = 1;
		pte->page_addr = page_addr >> 12;
		pte->r_w = 1;
		pte->u_s = 1;
	}
}

// créer une entrée de répertoire
static int create_page_dir(struct page_directory_entry *pde)
{
  if(memory_get_first_free_page() == NULL)
    return -1;
  paddr_t page_addr = memory_reserve_page_frame();

  last_page_table_next();
  
  pde->present = 1;
  pde->page_table_addr = page_addr >> 12; // check phys or virtual
  pde->r_w = 1;
	pde->u_s = 1;

  // map new PTE
  map(page_addr, get_last_page_table());

  return 0;
}

// Map dans le répertoire des pages une nouvelle page
static int map(paddr_t phys_page_addr, vaddr_t virt_page_addr)
{
  int dir = virt_page_addr >> 22;
  int table = (virt_page_addr >> 12) & 0x3FF;
  struct page_directory_entry * pde = get_pde(dir);
  
  if (!pde->present && create_page_dir(pde) == -1)
    return -1;
  
  create_page_entry(get_pte(dir, table), phys_page_addr);

  return 0;
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
  map(memory_reserve_page_frame(), get_end_page_directory());

  free_slabs.begin = (struct slab *) get_end_page_directory();
  free_slabs.begin->prev = NULL;
  free_slabs.begin->nb_pages = 1;
  free_slabs.begin->next = NULL;
  free_slabs.end = free_slabs.begin;

  used_slabs.begin = NULL;
  used_slabs.end = NULL;

  vmm_top = get_end_page_directory() + PAGE_SIZE;
}

// Agrandit le heap de nb_pages pages et ajoute le nouveau slab à la fin de free_pages
static int increase_heap(unsigned int nb_pages)
{
  unsigned int i;
  struct slab *slab = (struct slab *) vmm_top;

  for(i=0 ; i<nb_pages ; i++)
  {
    if(memory_get_first_free_page() == NULL)
      return -1;
    if(map(memory_reserve_page_frame(), vmm_top) == -1)
      return -1;
    vmm_top += PAGE_SIZE;
  }

  if(!is_empty(&free_slabs) &&
     (vaddr_t) free_slabs.end + free_slabs.end->nb_pages*PAGE_SIZE == vmm_top)
    free_slabs.end->nb_pages += nb_pages; // agrandit juste le dernier free slab
  else
  {
    slab->nb_pages = nb_pages;
    push_back(&free_slabs, slab);
  }

  return 0;
}

static int is_stuck(struct slab *s1, struct slab *s2)
{
  return s1 != NULL &&
         (vaddr_t) s1 + s1->nb_pages*sizeof(struct slab) == (vaddr_t) s2;
}

// Sépare un slab en deux quand il est trop grand et déplace le slab de free_pages vers used_pages
static void cut_slab(struct slab *s, unsigned int nb_pages)
{
  if(s->nb_pages > nb_pages) // cut slab
  {
    struct slab *new_slab =
      (struct slab*) ((vaddr_t) s + nb_pages*PAGE_SIZE);
    new_slab->nb_pages = s->nb_pages - nb_pages;
    new_slab->prev = s->prev;
    new_slab->next = s->next;

    s->nb_pages = nb_pages;
  } 

  remove(&free_slabs, s);
  add(&used_slabs, s);
}

// Alloue une page
unsigned int allocate_new_page(void **alloc)
{
  return allocate_new_pages(1, alloc);
}

// Alloue nb_pages pages qui sont placé en espace contigüe de la mémoire virtuelle 
unsigned int allocate_new_pages(unsigned int nb_pages, void **alloc)
{
  struct slab *slab = free_slabs.begin;

  while(slab != NULL && slab->nb_pages < nb_pages)
    slab = slab->next;

  if(slab == NULL)
  {
    if(increase_heap(nb_pages) == -1)
      return 0;

    slab = free_slabs.end;
  }

  cut_slab(slab, nb_pages);

  *(alloc) = (void *) ((vaddr_t) slab + sizeof(struct slab));
  return nb_pages*PAGE_SIZE - sizeof(struct slab); 
}

// Unallocate a page
int unallocate_page(void *page)
{
  struct slab *slab = used_slabs.begin;

  while(slab != NULL && (vaddr_t) slab + sizeof(struct slab) < (vaddr_t) page)
    slab = slab->next;

  if(slab == NULL)
    return -1;

  remove(&used_slabs, slab);
  add(&free_slabs, slab);
 
  // Fusionne 2 free_slab qui sont collé
  if(is_stuck(slab->prev, slab))
  {
    slab->prev->nb_pages += slab->nb_pages;
    remove(&free_slabs, slab);

    slab = slab->prev;
  }
  
  if(is_stuck(slab, slab->next))
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

  return 0;
}

// retourne le nombre de pages minimal à allouer pour une zone mémoire
// de taille size : entier_sup(size + overhead)
unsigned int calculate_min_pages(size_t size)
{
  double nb_pages = (double) (size + sizeof(struct slab)) / PAGE_SIZE;
  return (unsigned int) (nb_pages + ((nb_pages - (int) nb_pages > 0) ? 1 : 0));
}

static void print_slab(struct slab* s, bool free)
{
 // set_attribute(BLACK, free ? GREEN : RED);
  printf("[%x ; %x ; %x] ", s, s->prev, s->next);
  fflush(stdout);
 // reset_attribute();
}

void vmm_print_heap()
{
  struct slab *free_it = free_slabs.begin;
  struct slab *used_it = used_slabs.begin;

  printf("\n-- VMM : printing heap --\n");

  while(free_it != NULL && used_it != NULL)
  {
    if(free_it < used_it)
    {
      print_slab(free_it, TRUE);
      free_it = free_it->next;
    } else
    {
      print_slab(used_it, FALSE);
      used_it = used_it->next;
    }
  }

  while(free_it != NULL)
  {
    print_slab(free_it, TRUE);
    free_it = free_it->next;
  }

  while(used_it != NULL)
  {
    print_slab(used_it, FALSE);
    used_it = used_it->next;
  }
 
  printf("\nVMM TOP : %x\n", vmm_top);

  printf("-- VMM : end --\n");
}

