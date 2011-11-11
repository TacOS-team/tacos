/**
 * @file process.h
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010 - TacOS developers.
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

#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <types.h>
#include <elf.h>

/* Type d'exécution */
#define EXEC_ELF 0
#define EXEC_KERNEL 1

/* Structure à passer au noyau pour créer un nouveau processus */
typedef struct
{
	char* name;
	char* args;
	char** envp;
	
	int exec_type; /* Type d'exécution */
	
	void* data; /* Usage variable */
	
	Elf32_File* file;
	
	/* Données pour le chargement de l'elf */
	int mem_size;
	vaddr_t entry_point;
	
	uint32_t stack_size;
	int priority;
	
	uint16_t ppid;
	
}process_init_data_t;

void exec(paddr_t prog, char* name, int orphan);
int exec_elf(char* name, int orphan);

#endif
