#include <memory.h>
#include <pagination.h>

/*
 * Cette fonction sert à mapper une adresse par identity mapping. Cela ne peut être fait qu'avant l'activation de la pagination !
 */
void pagination_identity_map_addr(struct page_directory_entry * pagination_kernel, paddr_t page_addr) {
	static int c = 0;

	int index_pd = page_addr >> 22;
	int index_pt = (page_addr & 0x003FF000) >> 12;

	struct page_directory_entry * pde = &pagination_kernel[index_pd];
	if (!pde->present) {
		pde->r_w = 1;
		pde->present = 1;
		paddr_t pt_addr = memory_reserve_page_frame();
		pde->page_table_addr = pt_addr >> 12;
		pagination_identity_map_addr(pagination_kernel, pt_addr); // J'ai peur que si on ne map pas on se prenne un page fault (à voir...)
	}

	struct page_table_entry * page_table = (struct page_table_entry *) (pde->page_table_addr << 12);
	struct page_table_entry * pte = &page_table[index_pt];

	pte->present = 1;
	pte->page_addr = page_addr >> 12;
	pte->r_w = 1;
}

/*
 * Cette fonction s'occupe d'activer la pagination. (identity mapping entre autre qui permet de pas briser le lien @phys et @lineaire au début)
 */
void pagination_setup() {
	int i;

	struct physical_page_descr * iterator = memory_get_first_used_page();
	struct page_directory_entry * pagination_kernel = (struct page_directory_entry*) memory_reserve_page_frame();

	for (i = 0; i < 1023; i++) {
		pagination_kernel[i].present = 0;
	}

	// L'adresse linéaire 0xFFC00000 correspond au repertoire de tables de page.
	pagination_kernel[1023].page_table_addr = ((paddr_t)pagination_kernel) >> 12;
	pagination_kernel[1023].r_w = 1;
	pagination_kernel[1023].present = 1;

	// identity mapping :
	paddr_t current_page;
	while (memory_has_next_page(iterator)) {
		current_page = memory_next_page(&iterator);
		pagination_identity_map_addr(pagination_kernel, current_page);
	}
	
	// On place le rep dans le registre cr3
	asm volatile("mov %0, %%cr3":: "b"(pagination_kernel));

	//reads cr0, switches the "paging enable" bit, and writes it back.
	unsigned int cr0;
	asm volatile("mov %%cr0, %0": "=b"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0":: "b"(cr0));
	
}
