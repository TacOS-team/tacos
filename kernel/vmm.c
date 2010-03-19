#include <memory.h>
#include <pagination.h>
#include <vmm.h>

// Page Table Entry Magic
#define PTE_MAGIC 0x3FF

#define VMM_FREE_PAGES_MAX_SIZE 256

struct page
{
  uint32_t start; // debut d'une _page_
  size_t nb_pages;
} free_pages[VMM_FREE_PAGES_MAX_SIZE];

static int empty_cell(int it)
{
  return free_pages[it].nb_pages == 0;
}

// Retourne l'entrée de répertoire de page correspondant à dir
static struct page_directory_entry *get_pde(int dir)
{
  return ((struct page_directory_entry *) 
          (((struct page_directory_entry *) PTE_MAGIC)->page_table_addr)) + dir;
}

// Retourne l'entrée de table de page correspondant à dir, table
static struct page_table_entry *get_pte(int dir, int table)
{
  return ((struct page_table_entry *) get_pde(dir)->page_table_addr) + table;
}

// Retourne une adresse lineaire en fonction de sa position dans le rep de page
static uint32_t get_linear_address(int dir, int table, int offset)
{
  return (((dir&0x3FF) << 22) | ((table&0x3FF) << 12) | (offset&0xFFF));
}

// Ajoute un bloc de page dans la liste des free_pages
static int insert(uint32_t start, uint32_t end)
{
  int it = 0;
  while(it < VMM_FREE_PAGES_MAX_SIZE && !empty_cell(it))
    it++;

  if(it == VMM_FREE_PAGES_MAX_SIZE)
    return -1; // full table

  free_pages[it].start = start;
  free_pages[it].nb_pages = (start-end)/PAGE_SIZE;
  return 0;
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
	pde->present = 1;

  // XXX : MAP NEW PAGE TO A PTE
}

// Map dans le répertoire des pages une nouvelle page
static void map(paddr_t phys_page_addr, uint32_t virt_page_addr)
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

// sous fonction d'initialisation des pages libres
// explore un tableau de page
static int init_free_pages_iterate_pte(int dir, uint32_t start)
{
  int table;

  for(table=0 ; table<1023 ; table++)
  {
    struct page_table_entry *pte = get_pte(dir, table);

    if(start == 0 && !(pte->present))
      start = get_linear_address(dir, table, 0); 
    if(start != 0 && pte->present)
    {
      insert(start, get_linear_address(dir, table, 0));
      start = 0;
    }
  }

  return start;
}

// initialise la liste des pages libres en parcourant le répertoire des pages
static void init_free_pages()
{
  int it, dir;
  uint32_t start = 0;
 
  for(it=0 ; it<VMM_FREE_PAGES_MAX_SIZE ; it++)
  {
    free_pages[it].start = 0;
    free_pages[it].nb_pages = 0;
  }

  for(dir=0 ; dir<1023 ; dir++)
  {
    struct page_directory_entry *pde = get_pde(dir);
    if(pde->present)
      init_free_pages_iterate_pte(dir, start);
    else
    {
      if(start != 0)
      {
        insert(start, get_linear_address(dir, 0, 0));
        start = 0;
      }
    }
  }
}

void init_vmm()
{
  init_free_pages();
}

void *allocate_new_page()
{
  return allocate_new_pages(1);
}

// Alloue nb_pages pages qui sont placé en espace contigüe de la mémoire virtuelle 
void *allocate_new_pages(unsigned int nb_pages)
{
  uint32_t virt_addr = 0;
  unsigned int i, it;
  while(it < VMM_FREE_PAGES_MAX_SIZE && !empty_cell(it) &&
        free_pages[it].nb_pages < nb_pages)
    it++;

  virt_addr = free_pages[it].start;
  for(i=0 ; i<nb_pages ; i++)
  {
    paddr_t new_page = memory_reserve_page_frame();
    map(new_page, free_pages[it].start);
    free_pages[it].start = free_pages[it].start + PAGE_SIZE;
  }

  free_pages[it].nb_pages -= nb_pages;
  if(free_pages[it].nb_pages == 0)
    free_pages[it].start = 0;

  return (void *) virt_addr;
}

// Unallocate a page
int unallocate_page(void *page)
{
  int it=0;
  while(it < VMM_FREE_PAGES_MAX_SIZE)
  {
    if(empty_cell(it))
      continue;

    if(free_pages[it].start + PAGE_SIZE == (uint32_t) page)
    {
      free_pages[it].start = (uint32_t) page;
      free_pages[it].nb_pages++;
      return 0;
    }
    
    if(free_pages[it].start + free_pages[it].nb_pages*PAGE_SIZE == (uint32_t) page)
    {
      free_pages[it].nb_pages++;
      return 0;
    }
  }

  return -1;
}

