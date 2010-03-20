#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <types.h>
#include <gdt.h>
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
	uint16_t pid;
	uint8_t  state;
	uint8_t  priority;
	regs_t regs;
} process_t __attribute__ ((packed));

tss_t* get_default_tss();

int init_process(paddr_t prog, uint32_t argc, uint8_t** argv,process_t* new_proc);

int cpu_ctxt_init (paddr_t* pStack, paddr_t* pFunct);
void cpu_ctxt_switch(paddr_t* pOldStack, paddr_t* pNewSatck);

#endif
