#ifndef _MEMPAGE_H_
#define _MEMPAGE_H_

#include <types.h>

#define PAGE_SIZE 4096
#define BIOS_RESERVED_BASE 0xa0000
#define BIOS_RESERVED_TOP 0x100000

void memory_print_used_pages();
void memory_print_free_pages();
void memory_setup(size_t ram_size);
paddr_t memory_reserve_page_frame();
int memory_free_page_frame(paddr_t addr);

#endif
