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

/**
* @file process.h
*/
#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <libio.h>
#include <types.h>
#include <vmm.h>
#include <signal.h>
//#include <tty.h>

#define MAX_PROC 512

#define PROCSTATE_IDLE 1
#define PROCSTATE_RUNNING 2
#define PROCSTATE_WAITING 3
#define PROCSTATE_SUSPENDED 4
#define PROCSTATE_TERMINATED 5 

#define CPU_USAGE_SAMPLE_RATE 100

/* Type d'exécution */
#define EXEC_ELF 0
#define EXEC_KERNEL 1

/* Structure à passer au noyau pour créer un nouveau processus */
typedef struct
{
	char* name;
	char* args;
	
	int exec_type; /* Type d'exécution */
	
	void* data; /* Usage variable */
	
	/* Données pour le chargement de l'elf */
	int mem_size;
	vaddr_t entry_point;
	
	uint32_t stack_size;
	int priority;
	
	uint16_t ppid;
	
}process_init_data_t;

#define CURRENT_PROCESS -1

void exec(paddr_t prog, char* name, int orphan);
int exec_elf(char* name, int orphan);

#endif
