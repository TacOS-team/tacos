/**
* @file process.h
*/
#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <types.h>
#include <libio.h>
#include <pagination.h>

#define PROCSTATE_IDLE 1
#define PROCSTATE_RUNNING 2
#define PROCSTATE_WAITING 3
#define PROCSTATE_TERMINATED 4 

#define CPU_USAGE_SAMPLE_RATE 500

typedef struct
{
	uint32_t eax, ecx, edx, ebx;
	uint32_t esp, ebp, esi, edi;
	uint32_t eip, eflags;
	uint16_t cs, ss, ds, es, fs, gs;
	uint32_t cr3;
}regs_t;

/** 
* @brief 
*/
typedef struct{
	uint16_t	pid;
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
	struct {
		uint32_t esp0;
		uint16_t ss0;
	} kstack __attribute__ ((packed));
	/* Données utilisées pour les IO */
	file_descriptor fd[FOPEN_MAX];
	FILE* file_list;
	// Temporairement je rajoute stdin, stdout et stderr. 
	// Ça sera supprimé lorsqu'on aura un changement de la pagination lors du changement de contexte.
	FILE * stdin;
	FILE * stdout;
	FILE * stderr;

	struct page_directory_entry * pd;
} process_t;

typedef struct _proclist_cell{
	process_t* process;
	struct _proclist_cell* next;
	struct _proclist_cell* prev;
}*proc_list, proclist_cell;

/* Temporaire */
int create_process_test(char* name, paddr_t prog, uint32_t argc, char** argv, uint32_t prog_size, uint32_t stack_size, uint8_t ring __attribute__ ((unused)));

/** 
* @brief Crée un nouveau processus.
* Crée un nouveau processus et l'ajoute à la liste des processus avec un état d'exécution PROCSTATE_IDLE.
* @param name Nom du processus.
* @param prog Pointeur vers le point d'entrée du processus.
* @param argc Nombre d'arguments passés au processus.
* @param argv Tableau contenant les arguments à passer au processus.
* @param stack_size Taille de la pile utilisateur du processus.
* @param ring Niveau d'exécution du processus, toujours à 3.
* 
* @return Pid du processus créé.
*/
int create_process(char* name, paddr_t prog, uint32_t argc, char** argv, uint32_t stack_size, uint8_t ring);

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
* @brief Retourne le processus en cours d'utilisation
* 
* @return processus en cours d'utilisation.
*/
process_t* get_current_process();

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
* @brief Affiche la liste des processus.
*/
void print_process_list();

/** 
* @brief Nettoie la liste des processus.
* Retire de la liste tous les processus en état PROCSTATE_TERMINATED.
*/
void clean_process_list();

void sample_CPU_usage();

/** 
* @brief Donne le focus au prochain processus actif dans la liste.
*/
void change_active_process();
process_t* get_active_process();

/** 
* @brief Coté kernel de l'appel système kill.
* Change le statut du processus appelant à PROCSTATE_TERMINATED.
* @param ret_value ???
* @param zero1 Inutilisé
* @param zero2 Inutilisé
* 
* @return 
*/
void* sys_exit(uint32_t ret_value, uint32_t zero1, uint32_t zero2);
void* sys_getpid(uint32_t* pid, uint32_t zero1, uint32_t zero2);
void* sys_kill(uint32_t pid, uint32_t zero1, uint32_t zero2);

// A mettre en user-space
void exit(uint32_t value);
uint32_t get_pid();
void kill(uint32_t pid);
void exec(paddr_t prog, char* name);

#endif
