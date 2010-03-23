#include <types.h>
#include <stdlib.h>
#include <stdio.h> 


#include <gdt.h>
#include "process.h"
#include <kmalloc.h>

typedef int (*main_func_type) (uint32_t, uint8_t**);

uint32_t sys_stack[1024];
uint32_t user_stack[1024];
paddr_t ss_addr = sys_stack+1023;
paddr_t us_addr = user_stack+1023;

uint32_t proc_count = 0;

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


process_t* create_process(paddr_t prog, uint32_t argc, uint8_t** argv, uint32_t stack_size, uint8_t ring)
{
	uint32_t *sys_stack, *user_stack;
	process_t* new_proc;
	
	//asm("xchg %bx, %bx");
	new_proc = kmalloc(sizeof(process_t));
	sys_stack = kmalloc(stack_size*sizeof(uint32_t));
	user_stack = kmalloc(stack_size*sizeof(uint32_t));

	new_proc->pid = proc_count;
	new_proc->regs.eax = 0;
	new_proc->regs.ebx = 0;
	new_proc->regs.ecx = 0;
	new_proc->regs.edx = 0;
	
	if( ring == 0)
	{
		printf("Ring 0 process not implemented\n");
	}
	else
	{
		new_proc->regs.cs = 0x1B;
		new_proc->regs.ds = 0x23;
		new_proc->regs.ss = 0x23;
	}
	
	new_proc->regs.eflags = 0;
	new_proc->regs.eip = prog;
	new_proc->regs.esp = (user_stack)+stack_size;
	new_proc->sys_stack = (sys_stack)+stack_size;
	new_proc->state = PROCSTATE_IDLE;
	
	proc_count++;
	
	return new_proc;
}

void process_print_regs(process_t* current)
{
	printf("ss: 0x%x\n", current->regs.ss);
	printf("ss: 0x%x\n", current->regs.ss);		
	printf("esp: 0x%x\n", current->regs.esp);
	printf("flags: 0x%x\n", current->regs.eflags);
	printf("cs: 0x%x\n", current->regs.cs); 
	printf("eip: 0x%x\n", current->regs.eip); 
	printf("eax: 0x%x\n", current->regs.eax); 
	printf("ecx: 0x%x\n", current->regs.ecx); 
	printf("edx: 0x%x\n", current->regs.edx); 
	printf("ebx: 0x%x\n", current->regs.ebx); 
	printf("ebp: 0x%x\n", current->regs.ebp); 
	printf("esi: 0x%x\n", current->regs.esi); 
	printf("edi: 0x%x\n", current->regs.edi); 
	printf("fs: 0x%x\n", current->regs.fs); 
	printf("gs: 0x%x\n", current->regs.gs);  
	printf("ds: 0x%x\n", current->regs.ds); 
	printf("es: 0x%x\n", current->regs.es); 
	
	printf("0x%x\n",get_default_tss()->esp0);
}



