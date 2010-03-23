#ifndef _MEMPAGE_H_
#define _MEMPAGE_H_

#include <types.h>

#define PAGE_SIZE 4096
#define BIOS_RESERVED_BASE 0xa0000
#define BIOS_RESERVED_TOP 0x100000

struct physical_page_descr {
	/* Contient bien sûr l'adresse physique de la zone mémoire ! */
	paddr_t addr;

	/* Cet élément fera parti d'un liste doublement chainée (page utilisée ou libre) */
	struct physical_page_descr *next;
	struct physical_page_descr *prev;
};

void memory_print();
void memory_print_used_pages();
void memory_print_free_pages();
void memory_setup(size_t ram_size);
paddr_t memory_reserve_page_frame();
int memory_free_page_frame(paddr_t addr);

struct physical_page_descr * memory_get_first_used_page();
struct physical_page_descr * memory_get_first_free_page();
bool memory_has_next_page(struct physical_page_descr * iterator);
paddr_t memory_next_page(struct physical_page_descr ** iterator);

#endif
