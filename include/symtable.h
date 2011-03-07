#ifndef _SYMTABLE_H
#define _SYMTABLE_H

#include <types.h>

int load_symtable();
paddr_t sym_to_addr(char* symbol);
char* addr_to_sym(paddr_t addr);
int is_symtable_loaded();

#endif
