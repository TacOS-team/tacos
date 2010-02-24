#ifndef _PROCESS_H_
#define _PROCESS_H_


#include <types.h>

struct cpu_ctxt {
    uint16_t  gs;
    uint16_t  fs;
    uint16_t  ds;
    uint16_t  ss;
    uint16_t  cs; 
    uint16_t  alignement; //inutilisé

    uint32_t  edi;
    uint32_t  esi;
    uint32_t  ebp;
    uint32_t  esp;
    uint32_t  ebx;
    uint32_t  edx;
    uint32_t  ecx;
    uint32_t  eax;
    uint32_t  eflags;
    uint32_t  eip;

} __attribute__((packed));

struct process {
	uint16_t pid;
	uint8_t  state;
	uint8_t  priority;
	struct cpu_ctxt ctxt;
};

int init_process(paddr_t prog, uint32_t argc, uint8_t** argv, struct process* new_proc);

int cup_ctxt_init (paddr_t* pStack, paddr_t* pFunct);
void cpu_ctxt_switch(paddr_t* pOldStack, paddr_t* pNewSatck);

#endif
