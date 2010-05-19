/**
 * @file pagination.c
 */

#include <types.h>
#include <memory.h>
#include <pagination.h>

static vaddr_t last_page_table = 0;
static vaddr_t end_page_directory;

/** 
 * @brief Mappe une adresse en identity mapping.
 * 
 * Cette fonction sert à mapper une adresse par identity mapping. Cela ne peut
 * être fait qu'avant l'activation de la pagination !
 *
 * @param pagination_kernel
 * @param page_addr
 */
void pagination_identity_map_addr(struct page_directory_entry * pagination_kernel, paddr_t page_addr) {
	int index_pd = page_addr >> 22;
	int index_pt = (page_addr & 0x003FF000) >> 12;

	struct page_directory_entry * pde = &pagination_kernel[index_pd];
	if (!pde->present) {
		pde->r_w = 1;
		pde->u_s = 1;
		pde->present = 1;
		paddr_t pt_addr = memory_reserve_page_frame();
    if(last_page_table <= (vaddr_t) pt_addr)
      last_page_table = (vaddr_t) pt_addr;
		pde->page_table_addr = pt_addr >> 12;
		pagination_identity_map_addr(pagination_kernel, pt_addr); // J'ai peur que si on ne map pas on se prenne un page fault (à voir...)
	}

	struct page_table_entry * page_table = (struct page_table_entry *) (pde->page_table_addr << 12);
	struct page_table_entry * pte = &page_table[index_pt];

	pte->present = 1;
	pte->page_addr = page_addr >> 12;
	pte->r_w = 1;
	pte->u_s = 1;
}

void pagination_setup() {
	struct physical_page_descr * iterator = memory_get_first_used_page();
	struct page_directory_entry * pagination_kernel = (struct page_directory_entry*) memory_reserve_page_frame();

  // marque la fin du bloc regroupant le rep. de page et les tables de pages
  end_page_directory = ((vaddr_t) pagination_kernel) + 1025*PAGE_SIZE;

	pagination_init_page_directory(pagination_kernel);

	// identity mapping :
	paddr_t current_page;
	while (memory_has_next_page(iterator)) {
		current_page = memory_next_page(&iterator);
		if (pagination_kernel != (struct page_directory_entry*) current_page) {
			pagination_identity_map_addr(pagination_kernel, current_page);
		}
	}
	
	pagination_load_page_directory(pagination_kernel);

	//reads cr0, switches the "paging enable" bit, and writes it back.
	unsigned int cr0;
	asm volatile("mov %%cr0, %0": "=b"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0":: "b"(cr0));
}

void pagination_init_page_directory(struct page_directory_entry * pd) {
	int i;

	for (i = 0; i < 1023; i++) {
		pd[i].present = 0;
	}

	// La dernière entrée pointe vers lui même ce qui fait que :
	// 0xFFFFF00 est traduit par la MMU en l'adresse du page directory
	// 0xFFC + 1024 * index_page_table permet d'accéder à une pde
	pd[1023].page_table_addr = ((paddr_t)pd) >> 12;
	pd[1023].r_w = 1;
	pd[1023].present = 1;
	pd[1023].u_s = 1;
}

/*
 * Chargement d'un page directory
 */
void pagination_load_page_directory(struct page_directory_entry * pd) {
	// On place le rep dans le registre cr3
	asm volatile("mov %0, %%cr3":: "b"(pd));
}

/*
 * Retourne la fin du bloc regroupant le rep. de page et les tables de pages
 */
vaddr_t get_end_page_directory()
{
  return end_page_directory;
}

/*
 * Retourne l'addresse de la dernière table de pages
 */
vaddr_t get_last_page_table()
{
  return last_page_table;
}

int last_page_table_next()
{
  if(last_page_table >= end_page_directory)
    return -1;

  last_page_table += PAGE_SIZE;
  return 0;
}

