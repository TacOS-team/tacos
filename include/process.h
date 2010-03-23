#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <types.h>
#include <pagination.h>

#define PROCSTATE_IDLE 1
#define PROCSTATE_RUNNING 2
#define PROCSTATE_WAITING 3

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
} process_t;


process_t* create_process(paddr_t prog, uint32_t argc, uint8_t** argv, uint32_t stack_size, uint8_t ring);
void process_print_regs(process_t* current);

#endif
