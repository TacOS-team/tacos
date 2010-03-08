#include <memory.h>
#include <pagination.h>

/*
 * Cette fonction s'occupe d'activer la pagination. (identity mapping)
 */
void pagination_setup() {
	int i;

	pagination_kernel = (struct page_directory_entry*) memory_reserve_page_frame();

	for (i = 0; i < 1024; i++) {
		pagination_kernel[i].present = 0;
	}

	// Todo : identity mapping

	asm volatile("mov %0, %%cr3":: "b"(pagination_kernel));

	//reads cr0, switches the "paging enable" bit, and writes it back.
	unsigned int cr0;
	asm volatile("mov %%cr0, %0": "=b"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0":: "b"(cr0));
}
