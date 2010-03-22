#include <types.h>
#include <stdlib.h>
#include <stdio.h> 

#include "gdt.h"
#include "process.h"

typedef int (*main_func_type) (uint32_t, uint8_t**);

uint32_t sys_stack[1024];
uint32_t user_stack[1024];
paddr_t ss_addr = sys_stack+1023;
paddr_t us_addr = user_stack+1023;

void exec_task(main_func_type func, uint32_t argc, uint8_t** argv)
{
		asm(
		"cli\n\t"
		"push %3\n\t"
		"push %2\n\t"
		"push $0x23\n\t"		//SS
		"push %5\n\t"		//ESP
		"pushfl\n\t"
		"popl %%eax\n\t"
		"orl $0x200, %%eax\n\t"
		"and $0xffffbfff, %%eax\n\t"
		"push %%eax\n\t"	// Flags
		"push $0x1B\n\t"	//CS
		"push %0\n\t"		//EIP
		"movl %4, %1\n\t"	// Sauvegarde de la pile kernel dans la TSS
		"movw $0x20, %%ax\n\t" 
		"movw %%ax, %%ds\n\t"
		"iret"
		:"=m"(func), "=m"(get_default_tss()->esp): "m"(argc), "m"(argv), "a"(ss_addr), "b"(us_addr)
		);
}


int init_process(paddr_t prog, uint32_t argc, uint8_t** argv,process_t* new_proc)
{
	/* VIEUX CODE DE DEBUG
	int (*p)(uint32_t, uint8_t**) = (main_func_type)prog;
	p(argc,argv);
	*/
	
	/*
	 * non implemente
	 * TODO :
	 * mettre en place une stack
	 * mettre en place un segment data
	 * initialiser la stack (empiler les arguments...)
	 * remplir le struct process
	 */
	
	//new_proc->state = PROCSTATE_IDLE;
	new_proc->state = PROCSTATE_WAITING;
	
	return 1;
}



