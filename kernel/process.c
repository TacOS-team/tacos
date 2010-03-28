#include <types.h>
#include <stdlib.h>
#include <stdio.h> 


#include <gdt.h>
#include "process.h"
#include <kmalloc.h>

typedef int (*main_func_type) (uint32_t, uint8_t**);

uint32_t proc_count = 0;

static proc_list process_list = NULL;
static proclist_cell* current_proclist_cell = NULL;

process_t* get_current_process()
{
	return current_proclist_cell->process;
}

process_t* get_next_process()
{
	current_proclist_cell = current_proclist_cell->next;
	
	if(current_proclist_cell == NULL)
		current_proclist_cell = process_list;
		
		
	return current_proclist_cell->process;
}

void add_process(process_t* process)
{

	if(process_list == NULL)
	{
		process_list = kmalloc(sizeof(proclist_cell));
		process_list->prev = NULL;
		current_proclist_cell = process_list;
	}
	else
	{
		process_list->prev = kmalloc(sizeof(proclist_cell));

		process_list->prev->next= process_list;
		process_list = process_list->prev;
	}
	process_list->process = process;
	process_list->prev = NULL;
}

process_t* find_process(int pid)
{
	proclist_cell* aux = process_list;
	process_t* proc = NULL;
	
	while(proc==NULL && aux!=NULL)
	{
		if(aux->process->pid == pid)
			proc = aux->process;
		else
			aux = aux->next;
	} 
	return proc;
}

int delete_process(int pid)
{
	proclist_cell* aux = process_list;
	while(aux!=NULL && aux->process->pid!=pid)
	{
		aux = aux->next;
	} 
	if(aux==NULL) // Si aux==NULL, c'est qu'on a pas trouvé le process, on retourne -1
		return -1;
	
	aux->prev->next = aux->next;
	aux->next->prev = aux->prev;
	kfree(aux);
	
	proc_count--;
	
	return 0;
}

int create_process(paddr_t prog, uint32_t argc, uint8_t** argv, uint32_t stack_size, uint8_t ring)
{
	uint32_t *sys_stack, *user_stack;
	process_t* new_proc;

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
		new_proc->regs.cs = 0x8;
		new_proc->regs.ds = 0x10;
		new_proc->regs.ss = 0x10;
	}
	else
	{
		new_proc->regs.cs = 0x1B;
		new_proc->regs.ds = 0x23;
		new_proc->regs.ss = 0x23;
	}
	
	new_proc->regs.eflags = 0;
	new_proc->regs.eip = prog;
	new_proc->regs.esp = (user_stack)+stack_size-1;
	new_proc->sys_stack = (sys_stack)+stack_size-1;
	new_proc->state = PROCSTATE_IDLE;

	proc_count++;
	
	add_process(new_proc);
	
	return new_proc->pid;
}

void process_print_regs()
{
	process_t* current = get_current_process();
	
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

void print_process_list()
{
	proclist_cell* aux = process_list;
	while(aux!=NULL)
	{
		printf("pid:%d  state:",aux->process->pid);
		switch(aux->process->state)
		{
			case PROCSTATE_IDLE:
				printf("IDLE\n");
				break;
			case PROCSTATE_RUNNING:
				printf("RUNNING\n");
				break;
			case PROCSTATE_WAITING:
				printf("WAITING\n");
				break;
			case PROCSTATE_TERMINATED:
				printf("TERMINATED\n");
				break;
			default:
				break;
		}
		aux = aux->next;
	}
}


