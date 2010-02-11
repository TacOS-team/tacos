#ifndef _MEMPAGE_H_
#define _MEMPAGE_H_

#include <types.h>

#define PAGE_SIZE 4196
#define BIOS_RESERVED_BASE 0xa0000
#define BIOS_RESERVED_TOP 0x100000

void mempage_print_used_pages();
void mempage_print_free_pages();
void mempage_setup(size_t ram_size);


#endif
