/**
 * @file pagination.c
 */

#include <types.h>
#include <memory.h>
#include <pagination.h>
#include <stdio.h>

static vaddr_t last_page_table;

void pagination_map(struct page_directory_entry * pagination_kernel, paddr_t page_addr, vaddr_t v_page_addr) {
	int index_pd = v_page_addr >> 22;
	int index_pt = (v_page_addr & 0x003FF000) >> 12;

	struct page_directory_entry * pde = &pagination_kernel[index_pd];
	if (!pde->present) {
		int i;
		paddr_t pt_addr = memory_reserve_page_frame();
		pde->r_w = 1;
		pde->u_s = 1;
		pde->present = 1;
		pde->page_table_addr = pt_addr >> 12;

		struct page_table_entry *pt = (struct page_table_entry *) pt_addr;
		for (i = 0; i < 1024; i++)
			pt[i].present = 0;

		// On map la table de page
		pagination_map(pagination_kernel, pt_addr, get_last_page_table());
		last_page_table_next();
	}

	struct page_table_entry * page_table = (struct page_table_entry *) (pde->page_table_addr << 12);
	struct page_table_entry * pte = &page_table[index_pt];

	pte->present = 1;
	pte->page_addr = page_addr >> 12;
	pte->r_w = 1;
	pte->u_s = 1;
}

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
	pagination_map(pagination_kernel, page_addr, page_addr);
}

void pagination_setup() {
	struct physical_page_descr * iterator = memory_get_first_used_page();
	struct page_directory_entry * pagination_kernel = (struct page_directory_entry*) memory_reserve_page_frame();

	// Pile de mémoire virtuelle libre pour les tables de pages.
	// Placé en haut de la mémoire virtuelle (les 1024 premières sont
	// utilisé par le rep de page pour pointer vers lui-même.
	last_page_table = -1025*PAGE_SIZE;

	pagination_init_page_directory(pagination_kernel);
	pagination_map(pagination_kernel, (paddr_t) pagination_kernel, 
								 get_last_page_table());
	last_page_table_next();

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

void pagination_init_page_directory_from_current(struct page_directory_entry * pd) {
	int i;
	struct page_directory_entry * current = (struct page_directory_entry *)0xFFFFF000;

	for (i = 0; i < 1023; i++) {
		pd[i].present = current[i].present;
		pd[i].r_w = current[i].r_w;
		pd[i].u_s = current[i].u_s;
		pd[i].pwt = current[i].pwt;
		pd[i].pcd = current[i].pcd;
		pd[i].a = current[i].a;
		pd[i].ign1 = current[i].ign1;		
		pd[i].ps = current[i].ps;
		pd[i].ign2 = current[i].ign2;	
		pd[i].page_table_addr = current[i].page_table_addr;
	}

	// La dernière entrée pointe vers lui même ce qui fait que :
	// 0xFFFFF000 est traduit par la MMU en l'adresse du page directory
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
 * Retourne l'addresse de la dernière table de pages
 */
vaddr_t get_last_page_table()
{
	return last_page_table;
}

int last_page_table_next()
{
	last_page_table -= PAGE_SIZE;
	return 0;
}

