#include <types.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <libio.h> 

#include <gdt.h>
#include <process.h>
#include <kmalloc.h>
#include <syscall.h>
#include <time.h>

#include <debug.h>

typedef int (*main_func_type) (uint32_t, uint8_t**);

uint32_t proc_count = 0;

static proc_list process_list = NULL;
static proclist_cell* current_proclist_cell = NULL;

static process_t * active_process = (process_t*) 5242880;

uint32_t get_proc_count()
{
	return proc_count;
}

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
	
	if(aux->prev == NULL) // On est à la tete de la liste, alors on change directement "process_list"
		process_list = aux->next;
	else
		aux->prev->next = aux->next;
	
	if(aux->next != NULL) // On ne change ça que si on n'est pas en fin de liste
		aux->next->prev = aux->prev;
	
	kfree(aux);
	
	proc_count--;
	
	return 0;
}


int create_process(char* name, paddr_t prog, uint32_t argc, uint8_t** argv, uint32_t stack_size, uint8_t ring)
{
	uint32_t *sys_stack, *user_stack;
	process_t* new_proc;
	int i;
	
	new_proc = kmalloc(sizeof(process_t));
	if( new_proc == NULL )
	{
		kprintf("create_process: impossible de reserver la memoire pour le nouveau processus.\n");
		return -1;
	}
	new_proc->name = strdup(name);
	
	sys_stack = kmalloc(stack_size*sizeof(uint32_t));
	if( new_proc == NULL )
	{
		kprintf("create_process: impossible de reserver la memoire pour la pile systeme.\n");
		return -1;
	}
	
	user_stack = kmalloc(stack_size*sizeof(uint32_t));
	if( new_proc == NULL )
	{
		kprintf("create_process: impossible de reserver la memoire pour la pile utilisateur.\n");
		return -1;
	}
	
	new_proc->user_time = 0;
	new_proc->sys_time = 0;
	
	new_proc->pid = proc_count;
	new_proc->regs.eax = 0;
	new_proc->regs.ebx = 0;
	new_proc->regs.ecx = 0;
	new_proc->regs.edx = 0;

	if(ring == 0)
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
	new_proc->regs.esp = (user_stack)+stack_size-3;
	new_proc->regs.ebp = new_proc->regs.esp;
	new_proc->sys_stack = (sys_stack)+stack_size-1;
	new_proc->state = PROCSTATE_IDLE;
	
	/* Initialisation de la pile du processus */
	user_stack[stack_size-1]=(paddr_t)argv;
    user_stack[stack_size-2]=argc;
    user_stack[stack_size-3]=(paddr_t)exit;
	 
	/**(new_proc->regs.esp) = (uint32_t)1;
	*(new_proc->regs.esp) = (uint32_t)1;
	*(new_proc->regs.esp) = (uint32_t)1; */

	for(i=0;i<FOPEN_MAX;i++) 
		new_proc->fd[i].used = FALSE;
		
	init_stdfiles(&new_proc->stdin, &new_proc->stdout, &new_proc->stderr);
	proc_count++;
	
	add_process(new_proc);
	
	active_process = new_proc;
	
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
	
	const int clk_per_ms = CLOCKS_PER_SEC / 1000;
	long int ms;
	int s;
	int m;
	int h;
	int reste;
	
	while(aux!=NULL)
	{
		
		/* Calcul du temps d'execution du processus */
		ms = aux->process->user_time / clk_per_ms;
		s = ms / 1000;
		
		m = s / 60;
		s = s % 60;
		h = m / 60;
		m = m % 60;
		
		if (aux->process == active_process) {
			printf("*");
		}

		printf("pid:%d  name:%s time:%dh %dm %ds (stdin=%x) state:",aux->process->pid, aux->process->name, h, m ,s, aux->process->stdin);
		
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

void clean_process_list()
{
	proc_list temp = process_list;
	uint32_t pid;
	while(temp!=NULL)
	{
		if(temp->process->state == PROCSTATE_TERMINATED)
		{
			pid = temp->process->pid;
			if(current_proclist_cell->process->pid == temp->process->pid)
				get_next_process();
			temp = temp->next;
			delete_process(pid);
		}
		else
			temp = temp->next;
	}
}

process_t * get_active_process() {
	return active_process;
}

/*
 * met en active le prochain process qui n'est pas terminé.
 */
void change_active_process() {
	proclist_cell* aux = process_list;

	/* On cherche le aux qui pointe vers l'active_process */
	while (aux != NULL && aux->process != active_process) {
		aux = aux->next;
	}

	if (aux == NULL) {
		return;
	}

	/* On se place sur le suivant */
	aux = aux->next;

	/* On prend le premier qui n'est pas à l'état terminé */
	while (aux != NULL && aux->process->state == PROCSTATE_TERMINATED) {
		aux = aux->next;
	}

	if (aux == NULL) {
		aux = process_list;
		/* On cherche le aux qui pointe vers un terminé */
		while (aux != NULL 
				&& aux->process->state == PROCSTATE_TERMINATED 
				&& aux->process != active_process) {
			aux = aux->next;
		}
	}

	/* On change l'active process */
	active_process = aux->process;
	kprintf("active_process : %s\n", active_process->name);
}

/*
 * SYSCALL
 */

void* sys_exit(uint32_t ret_value, uint32_t zero1 __attribute__ ((unused)), uint32_t zero2 __attribute__ ((unused)))
{
	process_t* current;
	// On cherche le processus courant:
	current = get_current_process();
	
	// On a pas forcement envie de supprimer le processus immédiatement
	current->state = PROCSTATE_TERMINATED; 
	
	printf("DEBUG: exit(process %d returned %d)\n", current->pid, ret_value);

	if (current == active_process) {
		change_active_process();
	}

  return NULL;
}

void* sys_getpid(uint32_t* pid, uint32_t zero1 __attribute__ ((unused)), uint32_t zero2 __attribute__ ((unused)))
{
	process_t* process = get_current_process();
	*pid = process->pid;
	return NULL;
}

void* sys_kill(uint32_t pid, uint32_t zero1 __attribute__ ((unused)), uint32_t zero2 __attribute__ ((unused)))
{
	process_t* process = find_process(pid);
	
	// Violent? OUI!
	process->state = PROCSTATE_TERMINATED;
	
	return NULL;
}

/* A mettre en user space */
void exit(uint32_t value)
{
	syscall(0,value,0,0);
	while(1); // Pour ne pas continuer à executer n'importe quoi alors que le processus est sensé être arrété
}

uint32_t get_pid()
{
	int pid;
	syscall(1,(uint32_t)&pid, 0, 0);
	return pid;
}

void kill(uint32_t pid)
{
	syscall(4,pid,0,0);
}
