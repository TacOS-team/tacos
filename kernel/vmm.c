#include <memory.h>
#include <pagination.h>
#include <vmm.h>

// Page Table Entry Magic
#define PTE_MAGIC 0x3FF

#define VMM_FREE_PAGES_MAX_SIZE 256

struct page
{
  void *start; // debut d'une _page_
  size_t nb_pages;
} free_pages[VMM_FREE_PAGES_MAX_SIZE];
static int free_pages_it = 0;

static struct page_directory_entry *get_pde(int dir)
{
  return ((PTE_MAGIC)->page_table_addr +
          dir*sizeof(struct page_directory_entry));
}

static struct page_table_entry *get_pte(int dir, int table)
{
  return get_pde(dir)->page_table_entry + table*sizeof(struct page_table_entry);
}

static void *get_linear_address(int dir, int table, int offset)
{
  return (void *) ((dir&0x3FF) << 22) | ((table&0x3FF) << 12) | (offset&0xFFF);
}

static int free_pages_it_next()
{
  free_pages_it = (free_pages_it + 1) % VMM_FREE_PAGES_MAX_SIZE;
}

static int insert(uint32_t start, uint32_t end)
{
  while(free_pages[free_pages_it].start != -1)
    free_page_it_next();
  free_pages[free_pages_it] = {start, (start-end)/PAGE_SIZE};
}

static void create_page_entry(page_table_entry *pte, paddr_t page_addr)
{
  pte->present = 1;
	pte->page_addr = page_addr >> 12;
	pte->r_w = 1;
}

static void create_page_dir(page_directory_entry *pde)
{
	paddr_t page_addr = memory_reserve_page_frame();

  pde->present = 1;
	pde->page_table_addr = page_addr >> 12;
	pde->present = 1;

  // XXX : MAP NEW PAGE TO A PTE
}

// Map dans le répertoire des pages une nouvelle page
static void map(paddr_t phys_page_addr, void *virt_page_addr)
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
    struct page_directory_entry *page_table = get_pte(i, table);

    if(start == -1 && !(pte->present))
      start = get_linear_address(i, table, 0); 
    if(start != -1 && pte->present)
    {
      insert(start, get_linear_address(i, table, 0));
      start = -1;
    }
  }

  return start;
}

// initialise la liste des pages libres en parcourant le répertoire des pages
static void init_free_pages()
{
  int dir;
  uint32_t start = -1;
  
  for(dir=0 ; dir<1023 ; dir++)
  {
    struct page_directory_entry *page_dir = get_pde(dir);
    if(page_dir.present)
      init_free_pages_iterate_pte(dir, start);
    else
    {
      if(start = -1)
      {
        insert(start, get_linear_address(dir, 0, 0));
        start = -1;
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
  paddr_t page_addr = memory_reserve_page_frame();

  while(free_pages[free_pages_it].start != -1)
      free_page_it_next();

  free_pages[free_pages_it].start += PAGE_SIZE;
  free_pages[free_pages_it].nb_pages--;
  if(free_pages[free_pages_it].nb_pages == 0)
    free_pages[free_pages_it] = {-1, -1};
}

void *allocate_new_page()
{
  allocate_new_pages(1);
}

void *allocate_new_pages(int nb_pages)
{
  int i;
  while(free_pages[free_pages_it].start != -1 && 
        free_page_it[free_page_it].nb_pages < nb_pages)
    free_page_it_next();

  for(i=0 ; i<nb_pages ; i++)
  {
    paddr_t new_page = memory_reserve_page_frame();
    map(new_page, free_pages[free_pages_it].start);
    free_pages[free_pages_it].start =
      ((uint32_t) (free_pages[free_pages_it].start)) + PAGE_SIZE;
  }

  free_pages[free_pages_it].nb_pages -= nb_pages;
  if(free_pages[free_pages_it].nb_pages == 0)
    free_pages[free_pages_it] = {-1, -1};
}

