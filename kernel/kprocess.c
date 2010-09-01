#include <types.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <libio.h> 
#include <video.h>
#include <gdt.h>
#include <kprocess.h>
#include <kmalloc.h>
#include <ksyscall.h>
#include <elf.h>
#include <kfcntl.h>
#include <debug.h>

#define GET_PROCESS 0
#define GET_PROCESS_LIST 1
#define FIRST_PROCESS 0
#define NEXT_PROCESS 1
#define PREV_PROCESS 2

typedef int (*main_func_type) (uint32_t, uint8_t**);

uint32_t proc_count = 0;

static proc_list process_list = NULL;
static proclist_cell* current_proclist_cell = NULL;


proclist_cell* get_current_proclist_cell()
{
	return current_proclist_cell;
}

void set_current_proclist_cell(proclist_cell* cell)
{
	current_proclist_cell = cell;
}

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

/* Procédure permettant de construire le char** argv */
/* TODO: un schéma */
/* Valeur retournée: argc */
int arg_build(char* string, vaddr_t base, char*** argv_ptr)
{
	int argc = 1;
	int len = 0;
	int i;
	
	char* ptr = string;
	char** argv;
	char* str_copy_base;
	
	/* Première passe: on compte argc et le nombre total de caractères de la chaine, et on remplace les espaces par des 0*/
	while(*ptr != '\0')
	{
		/* Si on a un espace on a une sous-chaine, et on incrément argc */
		if(*ptr == ' ')
		{
			*ptr = '\0';
			argc++;
		}
		
		ptr++;
		len++;
	}
	
	argv = (char**) (base-(argc*sizeof(char*))); /* Base moins les argc cases pour les argv[x] */ 
	str_copy_base =(char*)(base-(argc*sizeof(char*)) - 1); /* Pareil, mais on va l'utiliser en descendant dans la mémoire, et non en montant comme avec un tableau */
	i = argc-1;
	
	*str_copy_base = '\0';
	ptr--;
	len--;
	
	while(len>=0)
	{
		/* Si on trouve '\0', c'est qu'on a fini de parcourir un mot, et on fait donc pointer l'argv comme il faut */
		if(*ptr=='\0')
		{
			argv[i] = str_copy_base+1;
			i--;
		}

			
		*str_copy_base = *ptr;
		str_copy_base--;
		ptr--;
		len--;
	}
	
	//*str_copy_base = *ptr;
	argv[0] = str_copy_base+1;
	*argv_ptr = argv;
	return argc;
}

process_t* create_process_elf(process_init_data_t* init_data)
{
	uint32_t *sys_stack, *user_stack;
	process_t* new_proc;
	
	char** argv;
	int argc;
	uint32_t* stack_ptr;
	
	vaddr_t temp_buffer;
	size_t program_size = init_data->mem_size;
	
	int i;
	int len;
	
	new_proc = kmalloc(sizeof(process_t));
	if( new_proc == NULL )
	{
		kprintf("create_process: impossible de reserver la memoire pour le nouveau processus.\n");
		return NULL;
	}
	len = strlen(init_data->name);
	new_proc->name = (char *) kmalloc((len+1)*sizeof(char));
	strcpy(new_proc->name, init_data->name);
	
	sys_stack = kmalloc(init_data->stack_size*sizeof(uint32_t));
	if( new_proc == NULL )
	{
		kprintf("create_process: impossible de reserver la memoire pour la pile systeme.\n");
		return NULL;
	}
	
	user_stack = kmalloc(init_data->stack_size*sizeof(uint32_t));
	if( new_proc == NULL )
	{
		kprintf("create_process: impossible de reserver la memoire pour la pile utilisateur.\n");
		return NULL;
	}
	
	/* Initialisation de la pile du processus */
	argc = arg_build(init_data->args,
	                (vaddr_t) &(user_stack[init_data->stack_size-1]), 
	                &argv);
	                
	stack_ptr = (uint32_t*) argv[0];
	
	*(stack_ptr-1) = (vaddr_t) argv;
	*(stack_ptr-2) = argc;
	*(stack_ptr-3) = (vaddr_t) exit;
	
	new_proc->ppid = init_data->ppid;
	
	new_proc->user_time = 0;
	new_proc->sys_time = 0;
	new_proc->current_sample = 0;
	new_proc->last_sample = 0;
	
	new_proc->pid = proc_count;
	new_proc->regs.eax = 0;
	new_proc->regs.ebx = 0;
	new_proc->regs.ecx = 0;
	new_proc->regs.edx = 0;

	new_proc->regs.cs = 0x1B;
	new_proc->regs.ds = 0x23;
	new_proc->regs.es = 0x0;
	new_proc->regs.fs = 0x0;
	new_proc->regs.gs = 0x0;
	new_proc->regs.ss = 0x23;
	
	new_proc->regs.eflags = 0;
	new_proc->regs.eip = init_data->entry_point;
	//new_proc->regs.esp = (vaddr_t)(user_stack)+stack_size-3;
	new_proc->regs.esp = (vaddr_t)(stack_ptr-3);
	new_proc->regs.ebp = new_proc->regs.esp;
	
	new_proc->kstack.ss0 = 0x10;
	//new_proc->kstack.esp0 = (vaddr_t)(sys_stack)+stack_size-1;
	new_proc->kstack.esp0 = (vaddr_t)(&sys_stack[init_data->stack_size-1]);
	
	new_proc->state = PROCSTATE_IDLE;
	
	new_proc->signal_data.mask = 0;
	new_proc->signal_data.pending_set = 0;
	new_proc->signal_data.stack.ss_size = 0x100;
	new_proc->signal_data.stack.ss_sp = kmalloc(0x100);
	
  // Initialisation des données pour la vmm
	new_proc->vm = (struct virtual_mem *) kmalloc(sizeof(struct virtual_mem));

	// Ne devrait pas utiliser kmalloc. Cf remarque suivante.
	new_proc->pd = kmalloc_one_aligned_page();
	paddr_t pd_paddr = vmm_get_page_paddr((vaddr_t) new_proc->pd);
	pagination_init_page_directory_copy_kernel_only(new_proc->pd, pd_paddr);

	/* On récupère l'exécutable temporairement dans le kernel (moche) */
	temp_buffer = (vaddr_t) kmalloc(program_size);
	memcpy((void*)temp_buffer,(void*)init_data->data, program_size);
	
	/* ZONE CRITIQUE */
	asm("cli");

	// Passer l'adresse physique et non virtuelle ! Attention, il faut que ça 
	// soit contigü en mémoire physique et aligné dans un cadre...
	pagination_load_page_directory((struct page_directory_entry *) pd_paddr);
	
	init_process_vm(new_proc->vm, calculate_min_pages(program_size));
	
	/* Copie du programme au bon endroit */
	memcpy((void*)0x40000000, (void*)temp_buffer, program_size);
	
	if(current_proclist_cell != NULL)
	{
		pd_paddr = vmm_get_page_paddr((vaddr_t) get_current_process()->pd);
		pagination_load_page_directory((struct page_directory_entry *) pd_paddr);
	}
	
	for(i=0;i<FOPEN_MAX;i++) 
		new_proc->fd[i].used = FALSE;

	/* Initialisation des entrées/sorties standards */
	init_stdfd(new_proc);
	// Plante juste après le stdfd avec qemu lorsqu'on a déjà créé 2 process. Problème avec la mémoire ?
	proc_count++;
	
	/* FIN ZONE CRITIQUE */
	asm("sti");	
	
	kfree((void*)temp_buffer);
	
	return new_proc;
}

process_t* create_process(process_init_data_t* init_data)
{
	char* name = init_data->name;
	paddr_t prog = (paddr_t) init_data->data;
	char* param = init_data->args;
	uint32_t stack_size = init_data->stack_size;

	uint32_t *sys_stack, *user_stack;
	process_t* new_proc;
	
	char** argv;
	int argc;
	uint32_t* stack_ptr;
	
	int i;
	int len;
	
	new_proc = kmalloc(sizeof(process_t));
	if( new_proc == NULL )
	{
		kprintf("create_process: impossible de reserver la memoire pour le nouveau processus.\n");
		return NULL;
	}
	len = strlen(name);
	new_proc->name = (char *) kmalloc((len+1)*sizeof(char));
	strcpy(new_proc->name, name);
	
	sys_stack = kmalloc(stack_size*sizeof(uint32_t));
	if( new_proc == NULL )
	{
		kprintf("create_process: impossible de reserver la memoire pour la pile systeme.\n");
		return NULL;
	}
	
	user_stack = kmalloc(stack_size*sizeof(uint32_t));
	if( new_proc == NULL )
	{
		kprintf("create_process: impossible de reserver la memoire pour la pile utilisateur.\n");
		return NULL;
	}
	
	/* Initialisation de la pile du processus */
	argc = arg_build(param,(vaddr_t) &(user_stack[stack_size-1]), &argv);
	stack_ptr = (uint32_t*) argv[0];
	
	*(stack_ptr-1) = (vaddr_t) argv;
	*(stack_ptr-2) = argc;
	*(stack_ptr-3) = (vaddr_t) exit;
	
	new_proc->ppid = init_data->ppid;
	
	new_proc->user_time = 0;
	new_proc->sys_time = 0;
	new_proc->current_sample = 0;
	new_proc->last_sample = 0;
	
	new_proc->pid = proc_count;
	new_proc->regs.eax = 0;
	new_proc->regs.ebx = 0;
	new_proc->regs.ecx = 0;
	new_proc->regs.edx = 0;

	new_proc->regs.cs = 0x1B;
	new_proc->regs.ds = 0x23;
	new_proc->regs.ss = 0x23;
	
	new_proc->regs.eflags = 0;
	new_proc->regs.eip = prog;
	//new_proc->regs.esp = (vaddr_t)(user_stack)+stack_size-3;
	new_proc->regs.esp = (vaddr_t)(stack_ptr-3);
	new_proc->regs.ebp = new_proc->regs.esp;
	
	new_proc->kstack.ss0 = 0x10;
	//new_proc->kstack.esp0 = (vaddr_t)(sys_stack)+stack_size-1;
	new_proc->kstack.esp0 = (vaddr_t)(&sys_stack[stack_size-1]);
	
	new_proc->state = PROCSTATE_IDLE;
	
	/* Initialisation des signaux */
	for(i=0; i<NSIG; i++)
		new_proc->signal_data.handlers[i] = NULL;
	
	new_proc->signal_data.mask = 0;
	new_proc->signal_data.pending_set = 0;
	new_proc->signal_data.stack.ss_size = 0x100;
	new_proc->signal_data.stack.ss_sp = kmalloc(0x100);
	
  // Initialisation des données pour la vmm
	new_proc->vm = (struct virtual_mem *) kmalloc(sizeof(struct virtual_mem));

	// Ne devrait pas utiliser kmalloc. Cf remarque suivante.
	new_proc->pd = kmalloc_one_aligned_page();
	paddr_t pd_paddr = vmm_get_page_paddr((vaddr_t) new_proc->pd);
	pagination_init_page_directory_copy_kernel_only(new_proc->pd, pd_paddr);

	/* ZONE CRITIQUE */
	asm("cli");

	// Passer l'adresse physique et non virtuelle ! Attention, il faut que ça 
	// soit contigü en mémoire physique et aligné dans un cadre...
	pagination_load_page_directory((struct page_directory_entry *) pd_paddr);
	
	init_process_vm(new_proc->vm, 1);
	
	if(current_proclist_cell != NULL)
	{
		pd_paddr = vmm_get_page_paddr((vaddr_t) get_current_process()->pd);
		pagination_load_page_directory((struct page_directory_entry *) pd_paddr);
	}

	for(i=0;i<FOPEN_MAX;i++) 
		new_proc->fd[i].used = FALSE;

	/* Initialisation des entrées/sorties standards */
	init_stdfd(new_proc);
	// Plante juste après le stdfd avec qemu lorsqu'on a déjà créé 2 process. Problème avec la mémoire ?
	proc_count++;

	/* FIN ZONE CRITIQUE */
	asm("sti");	
	
	return new_proc;
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

void sample_CPU_usage()
{
	proclist_cell* aux = process_list;
	while(aux!=NULL)
	{
		aux->process->last_sample = aux->process->current_sample;
		aux->process->current_sample = 0;
		aux = aux->next;
	}
}

/*
 * SYSCALL
 */

SYSCALL_HANDLER1(sys_exit,uint32_t ret_value __attribute__ ((unused)))
{
	process_t* current;
	// On cherche le processus courant:
	current = get_current_process();
	
	// On a pas forcement envie de supprimer le processus immédiatement
	current->state = PROCSTATE_TERMINATED; 
	
	//kprintf("DEBUG: exit(process %d returned %d)\n", current->pid, ret_value);
}

SYSCALL_HANDLER1(sys_getpid, uint32_t* pid)
{
	process_t* process = get_current_process();
	*pid = process->pid;
}

SYSCALL_HANDLER1(sys_exec, process_init_data_t* init_data)
{
	if(init_data->exec_type == EXEC_KERNEL)
		add_process(create_process(init_data));
	else
		add_process(create_process_elf(init_data));
}

process_t* sys_proc_list(uint32_t action)
{
	static proclist_cell* aux = NULL;
	process_t* ret = NULL;
	switch(action)
	{
		case FIRST_PROCESS:
			aux = process_list;	
			break;
		case NEXT_PROCESS:
			if(aux != NULL)
			{
				aux = aux->next;
			}
			break;
		case PREV_PROCESS:
			if(aux != NULL)
			{
				aux = aux->next;
			}
			break;
		default:
			kprintf("sys_proc_list: invalid action.\n");
	}
	
	if( aux != NULL )
		ret = aux->process;
		
	return ret;
}

SYSCALL_HANDLER3(sys_proc, uint32_t sub_func, uint32_t param1, uint32_t param2)
{
	switch(sub_func)
	{
		case GET_PROCESS:
			if((int)param1 == CURRENT_PROCESS)
				*((process_t**) param2) = get_current_process();
			else
				*((process_t**) param2) = find_process(param1);
			break;
			
		case GET_PROCESS_LIST:
			*((process_t**) param2) = sys_proc_list(param1);
			break;
			
		default:
			kprintf("sys_proc: invalid syscall.\n");
	}
}
