#include <memory.h>
#include <pagination.h>

/*
 * Cette fonction s'occupe d'activer la pagination. (identity mapping)
 */
void pagination_setup() {
	int i;

	struct physical_page_descr * iterator = memory_get_first_used_page();
	struct page_directory_entry * pagination_kernel = (struct page_directory_entry*) memory_reserve_page_frame();

	for (i = 0; i < 1023; i++) {
		pagination_kernel[i].present = 0;
	}

	// L'adresse linéaire 0xFFFFF0000 correspond au repertoire de tables de page.
	pagination_kernel[1023].page_table_addr = ((paddr_t)pagination_kernel) & (0xFFFFF);
	pagination_kernel[1023].r_w = 1;
	pagination_kernel[1023].present = 1;

	// identity mapping :
	while (memory_has_next_page(iterator)) {
		memory_next_page(&iterator);
	}

	// On place le rep dans le registre cr3
	asm volatile("mov %0, %%cr3":: "b"(pagination_kernel));

	//reads cr0, switches the "paging enable" bit, and writes it back.
	unsigned int cr0;
	asm volatile("mov %%cr0, %0": "=b"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0":: "b"(cr0));
}
