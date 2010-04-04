#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <types.h>
#include <pagination.h>
#include <libio.h>

#define PROCSTATE_IDLE 1
#define PROCSTATE_RUNNING 2
#define PROCSTATE_WAITING 3
#define PROCSTATE_TERMINATED 4

typedef struct
{
	uint32_t eax, ecx, edx, ebx;
	uint32_t esp, ebp, esi, edi;
	uint32_t eip, eflags;
	uint16_t cs, ss, ds, es, fs, gs;
	uint32_t cr3;
}regs_t;


typedef struct{
	uint16_t	pid;
	uint8_t	state;
	uint8_t	priority;
	paddr_t		sys_stack;
	regs_t regs;

	file_descriptor fd[FOPEN_MAX];
	FILE* file_list;
	// Temporairement je rajoute stdin, stdout et stderr. 
	// Ça sera supprimé lorsqu'on aura un changement de la pagination lors du changement de contexte.
	FILE * stdin;
	FILE * stdout;
	FILE * stderr;
} process_t;

typedef struct _proclist_cell{
	process_t* process;
	struct _proclist_cell* next;
	struct _proclist_cell* prev;
}*proc_list, proclist_cell;

int create_process(paddr_t prog, uint32_t argc, uint8_t** argv, uint32_t stack_size, uint8_t ring);
int delete_process(int pid);
process_t* find_process(int pid);
process_t* get_current_process();
process_t* get_next_process();
uint32_t get_proc_count();
void process_print_regs();
void print_process_list();
void clean_process_list();
process_t* get_active_process();

void* sys_exit(uint32_t ret_value, uint32_t zero1, uint32_t zero2);
void* sys_getpid(uint32_t* pid, uint32_t zero1, uint32_t zero2);
void* sys_kill(uint32_t pid, uint32_t zero1, uint32_t zero2);

// A mettre en user-space
void exit(uint32_t value);
uint32_t get_pid();
void kill(uint32_t pid);

#endif
