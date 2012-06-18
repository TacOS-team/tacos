/**
 * @file kprocess.h
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
 * @brief Création de nouveaux processus.
 */

#ifndef _KPROCESS_H_
#define _KPROCESS_H_

#define USER_PROCESS_BASE 0x40000000

#include <ksyscall.h>
#include <fd_types.h>
#include <signal_types.h>
#include <process_types.h>
#include <symtable.h>
#include <types.h>
#include <vmm.h>

#define MAX_PROC 512

#define CPU_USAGE_SAMPLE_RATE 100

#define PROCSTATE_IDLE 0
#define PROCSTATE_RUNNING 1
#define PROCSTATE_WAITING 2
#define PROCSTATE_SUSPENDED 3
#define PROCSTATE_TERMINATED 4

#define CURRENT_PROCESS -1

/* Type d'exécution */
#define EXEC_ELF 0
#define EXEC_KERNEL 1

/**
 * Structure à passer au noyau pour créer un nouveau processus
 */
typedef struct
{
	char* name; /**< Nom du processus. */
	char* args; /**< Ses arguments. */
	char** envp; /**< Ses variables d'environnement. */
	
	int exec_type; /**< Type d'exécution */
	
	void* data; /**< Usage variable */
	
	Elf32_File* file;
	
	/* Données pour le chargement de l'elf */
	int mem_size;
	vaddr_t entry_point;
	
	uint32_t stack_size;
	int priority;
	
	uint16_t ppid;
	
} process_init_data_t;

struct _tty_struct_t;

/** 
* @brief 
*/
typedef struct{
	uint16_t	pid;
	uint16_t	ppid;
	char* 		name;
	uint8_t	state;
	uint8_t	priority;
	
	/* Données dédiées au évaluation de perf */
	long int	user_time;
	long int	sys_time;
	int current_sample;
	int last_sample;
	
	/* Données propres au contexte du processus */
	regs_t regs;
	
	/* Données utilisées pour les IO */
	file_descriptor fd[FOPEN_MAX];
	
	struct page_directory_entry * pd;
	struct virtual_mem *vm;
	
	signal_process_data_t signal_data;

	char *ctrl_tty;
	
	/* Données liées au debug run-time du process */
	symbol_table_t* symtable;
	
	int sem_wait;

} process_t;

/**
 * Cellule de la liste des processus.
 */
typedef struct _proclist_cell{
	process_t* process;
	struct _proclist_cell* next;
	struct _proclist_cell* prev;
}*proc_list, proclist_cell;


void init_process_array();

/** 
* @brief (OUTDATED)Crée un nouveau processus.
* Crée un nouveau processus et l'ajoute à la liste des processus avec un état d'exécution PROCSTATE_IDLE.
* @param init_data informations sur le processus à créer
* @see process_init_data_t
* 
* @return Pid du processus créé.
*/
process_t* create_process(process_init_data_t* init_data);

/** 
* @brief Retire un processus de la liste.
* 
* @param pid Pid du processus à retirer de la liste.
* 
* @return 0 la plupart du temps.
*/
int delete_process(int pid);


/** 
* @brief Cherche le process_t* en correspondant à un pid donné.
*
* @param pid Pid du processus à chercher.
* 
* @return process_t* correspondant au pid.
*/
process_t* find_process(int pid);

/** 
* @brief Retourne le processus suivant le processus courant dans la liste
* 
* @return processus suivant.
*/
process_t* get_next_process();

/** 
* @brief Retourne le nombre de processus dans la liste.
* 
* @return nombre de processus dans la liste.
*/
uint32_t get_proc_count();

/** 
* @brief Nettoie la liste des processus.
* Retire de la liste tous les processus en état PROCSTATE_TERMINATED.
*/
void clean_process_list();

void sample_CPU_usage();

SYSCALL_HANDLER1(sys_exit,uint32_t ret_value __attribute__ ((unused)));
SYSCALL_HANDLER1(sys_getpid, uint32_t* pid);
SYSCALL_HANDLER1(sys_getppid, uint32_t* ppid);
SYSCALL_HANDLER2(sys_exec, process_init_data_t* init_data, int *pid);
SYSCALL_HANDLER3(sys_proc, uint32_t sub_func, uint32_t param1, uint32_t param2);
SYSCALL_HANDLER1(sys_waitpid, int pid);

void add_process(process_t* process);

void inject_idle(process_t* proc);

void procfs_init();

process_t* get_process_array(int i);
#endif /* _K_PROCESS_H_ */
