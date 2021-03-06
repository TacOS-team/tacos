/**
 * @file kprocess.h
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2015 TacOS developers.
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

/**
 * Adresse de debut du heap.
 */
#define USER_PROCESS_BASE 0x40000000

/**
 * Adresse de debut de la stack (Attention, part vers le bas !)
 */
#define USER_PROCESS_STACK 0xFF000000

/**
 * Adresse maximale pour un mmap (alloue le plus haut possible)
 */
#define USER_PROCESS_MMAP 0xFE800000

#include <ksyscall.h>
#include <fd_types.h>
#include <signal_types.h>
#include <process_types.h>
#include <symtable.h>
#include <types.h>

#define MAX_PROC 512 /**< Nombre max de processus. */

#define CPU_USAGE_SAMPLE_RATE 100

/**
 * États possibles des process.
 */
#define PROCSTATE_IDLE 0 /**< Inactif. */
#define PROCSTATE_RUNNING 1 /**< En cours d'exécution. */
#define PROCSTATE_WAITING 2 /**< En attente. */
#define PROCSTATE_SUSPENDED 3 /**< Suspendu. */
#define PROCSTATE_TERMINATED 4 /**< Terminé. */

#define CURRENT_PROCESS -1

/* Type d'exécution */
#define EXEC_ELF 0
#define EXEC_KERNEL 1

/* Données dédiées au évaluation de perf */
extern unsigned long long user_time;
extern unsigned long long sys_time;
extern unsigned long long idle_time;

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
	
	uint16_t ppid; /**< Parent process ID */
} process_init_data_t;

struct _tty_struct_t;

/** 
* @brief Structure représentant un processus.
*/
typedef struct process {
	uint16_t	pid; /**< Process ID */
	uint16_t	ppid; /**< Parent process ID */
	char* 		name; /**< Nom du processus. */
	uint8_t	state; /**< Etat du processus (idle, running, waiting...) */
	//uint8_t	priority; // Not used.
	uint8_t sig_interruptable; /**< 1 si process peut être interrompu par signal */
	
	/* Données dédiées au évaluation de perf */
	unsigned long long user_time;
	unsigned long long sys_time;
	
	char *cwd; /**< Current Working Directory */

	/* Données propres au contexte du processus */
	regs_t regs;
	
	/* Données utilisées pour les IO */
	open_file_descriptor* fd[FOPEN_MAX];
	
	struct page_directory_entry * pd;
	struct virtual_mem *vm;
	struct mmap_region* list_regions;
	struct physical_page_descr *reserved_pages;
	
	signal_process_data_t signal_data; /**< Signaux bloqués / à traiter / handlers associés */

	char *ctrl_tty; /**< Controlling tty. */
	
	/* Données liées au debug run-time du process */
	symbol_table_t* symtable;

	/**
	 * Sémaphore initialisé à 0 et qui est pris par les
	 * processus en attente de la fin de ce process.
	 */
	int sem_wait;

	/**
	 * Sémaphore qui passe à 1 lorsqu'un fils se termine.
	 */
	int sem_wait_child;

	/**
	 * Liste des fils
	 */
	int nb_children;
	struct process * children[MAX_PROC];

} process_t;

/**
 * Cellule de la liste des processus.
 */
typedef struct _proclist_cell{
	process_t* process; /**< Processus. */
	struct _proclist_cell* next; /**< Maillon suivant. */
	struct _proclist_cell* prev; /**< Maillon précedent. */
}*proc_list, proclist_cell;

/** 
* @brief Crée un nouveau processus kernel.
* Crée un nouveau processus kernel et l'ajoute à la liste des processus avec un état d'exécution PROCSTATE_IDLE.
* @param name nom du process
* @param entry_point adresse du point d'entrée
* @param stack_size taille de la pile à allouer
* 
* @return PID du processus
*/
int create_kprocess(char* name,
                    void* entry_point,
                    uint32_t stack_size);



process_t* create_process(process_init_data_t* init_data, uint8_t isKernel);

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

/**
 * @brief Syscall appelé lors de la fin d'exécution d'un process.
 */
SYSCALL_HANDLER1(sys_exit,uint32_t ret_value __attribute__ ((unused)));

/**
 * @brief Récupére le PID du process courant.
 *
 * @param pid adresse où enregistrer le pid.
 */
SYSCALL_HANDLER1(sys_getpid, uint32_t* pid);
/**
 * @brief Récupére le PID du process parent.
 *
 * @param ppid adresse où enregistrer le ppid.
 */
SYSCALL_HANDLER1(sys_getppid, uint32_t* ppid);

/**
 * @brief Syscall pour exécuter un programme.
 *
 * @param cmdline Ligne de commande à exécuter.
 * @param environ Variables d'environnement.
 * @param retval Pointeur servant à retourner le résultat.
 */
SYSCALL_HANDLER3(sys_exec, char *cmdline, char **environ, int *retval);

/**
 * @brief Bloque le process courant jusqu'à ce que le processus identifié par pid se termine.
 *
 * @param pid PID du process à attendre.
 * @param ret valeur de retour.
 */
SYSCALL_HANDLER2(sys_waitpid, int pid, int* ret);

/**
 * @brief Bloque le process courant jusqu'à ce que le processus identifié par pid se termine.
 *
 * @param pid PID du process à attendre.
 * @return valeur de retour.
 */
int waitpid(int pid);

/**
 * @brief Initialisation du proc FS.
 */
void procfs_init();

/**
 * @brief Récupère un process à partir de sa position dans le tableau.
 *
 * @param i indice du process dans le tableau (attention, ce n'est pas son pid)
 */
process_t* get_process_array(int i);

paddr_t reserve_page_frame(process_t* process);
void release_page_frames(process_t* process);

#endif /* _K_PROCESS_H_ */
