#include <memory.h>
#include <pagination.h>
#include <vmm.h>

// Page Table Entry Magic
#define PTE_MAGIC 0x3FF

struct page
{
  uint32_t start; // debut d'une _page_
  uint32_t end;   // fin d'une _page_
  struct page *next;
  struct page *prev;
} *free_pages;

static struct page_directory_entry *get_pde(int dir)
{
  return ((PTE_MAGIC)->page_table_addr +
          dir*sizeof(struct page_directory_entry));
}

static struct page_table_entry *get_pte(int dir, int table)
{
  return get_pde(dir)->page_table_entry + table*sizeof(struct page_table_entry);
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

static void init_free_pages()
{
  int i, j;
  paddr_t new_page = memory_reserve_page_frame();
  uint32_t start = -1, end = -1;

  for(i=0 ; i<1023 ; i++)
  {
    struct page_directory_entry *page_dir = get_pde(i); 
    if(page_dir[i].present)
    {
      for(j=0 ; j<1023 ; j++)
      {
        struct page_directory_entry *page_table = get_pte(i, j);

        if(start == -1 && !(pte->present))
          start = // XXX : Page linear address
        if(start != -1 && pte->present)
        {
          end = // XXX : Page linear address
        }
      }
    }else
    {

    }
  }

}

void init_vmm()
{
  page_directory_entry *pde = (

  init_free_pages();

  while(PDE != PTE_MAGIC)
  {
    page_table_entry *pte = pte->page_table_addr;
     
  }
}

void *allocate_new_page()
{
  paddr_t page_addr = memory_reserve_page_frame();

}

