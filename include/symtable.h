#ifndef _SYMTABLE_H
#define _SYMTABLE_H

#include <types.h>
#include <elf.h>

typedef struct
{
	char* name;
	paddr_t addr;
}symbol_t;

typedef struct {
	symbol_t* symbols;
	int count;
}symbol_table_t;

symbol_table_t* load_symtable(Elf32_File* file);

void load_kernel_symtable();

paddr_t sym_to_addr(symbol_table_t* table, char* symbol);

char* addr_to_sym(symbol_table_t* table, paddr_t addr);


#endif
