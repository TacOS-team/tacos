/**
 * @file symtable.h
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012 - TacOS developers.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * @section DESCRIPTION
 *
 * Description de ce que fait le fichier
 */
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
