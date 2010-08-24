/**
* @file process.h
*/
#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <libio.h>
#include <pagination.h>
#include <types.h>
#include <vmm.h>

#define PROCSTATE_IDLE 1
#define PROCSTATE_RUNNING 2
#define PROCSTATE_WAITING 3
#define PROCSTATE_TERMINATED 4 

#define CPU_USAGE_SAMPLE_RATE 1000

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
	struct virtual_mem *vm;
} process_t;

typedef struct _proclist_cell{
	process_t* process;
	struct _proclist_cell* next;
	struct _proclist_cell* prev;
}*proc_list, proclist_cell;

#define CURRENT_PROCESS -1

void exit(uint32_t value);

uint32_t get_pid();

void kill(uint32_t pid);

void exec(paddr_t prog, char* name);

process_t* get_process(int pid);

/* Actions possibles pour get_process_list */
#define FIRST_PROCESS 0
#define NEXT_PROCESS 1
#define PREV_PROCESS 2

process_t* get_process_list(uint32_t action);

/* Temporairement exposé */
int create_process_test(char* name, paddr_t* prog_addr, uint32_t prog_size, char* param, uint32_t stack_size, uint8_t ring __attribute__ ((unused)));

#endif
