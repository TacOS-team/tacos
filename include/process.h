/**
* @file process.h
*/
#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <libio.h>
#include <pagination.h>
#include <types.h>
#include <vmm.h>
#include <signal.h>
//#include <tty.h>

#define MAX_PROC 512

#define PROCSTATE_IDLE 1
#define PROCSTATE_RUNNING 2
#define PROCSTATE_WAITING 3
#define PROCSTATE_TERMINATED 4 

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
	struct {
		uint32_t esp0;
		uint16_t ss0;
	} kstack __attribute__ ((packed));
	/* Données utilisées pour les IO */
	file_descriptor fd[FOPEN_MAX];
	FILE* file_list;
	
    struct page_directory_entry * pd;
	struct virtual_mem *vm;
	
	signal_process_data_t signal_data;

    //XXX: terminal_t *ctrl_tty;
} process_t;

typedef struct _proclist_cell{
	process_t* process;
	struct _proclist_cell* next;
	struct _proclist_cell* prev;
}*proc_list, proclist_cell;

#define CURRENT_PROCESS -1

void exit(uint32_t value);

uint32_t get_pid();

void exec(paddr_t prog, char* name, int orphan);
int exec_elf(char* name, int orphan);

process_t* get_process(int pid);

/* Actions possibles pour get_process_list */
#define FIRST_PROCESS 0
#define NEXT_PROCESS 1
#define PREV_PROCESS 2

process_t* get_process_list(uint32_t action);

#endif
