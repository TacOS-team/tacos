#include <types.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <libio.h> 
#include <video.h>
#include <gdt.h>
#include <kprocess.h>
#include <kmalloc.h>
#include <syscall.h>
#include <elf.h>

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

static process_t * active_process = NULL; //à un moment il fallait faire ça pour que ça ne plante pas : (process_t*) 5242880; // Tant qu'il y a 42 dans l'adresse, ça me va.

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
	
	kprintf("Analyse de %s...\n", ptr);
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

int create_process_test(char* name, paddr_t* prog_addr, uint32_t prog_size, char* param, uint32_t stack_size, uint8_t ring __attribute__ ((unused)))
{
	uint32_t *sys_stack, *user_stack;
	process_t* new_proc;
	
	char** argv;
	int argc;
	uint32_t* stack_ptr ;
	
	int i;
	int len;
	
	new_proc = kmalloc(sizeof(process_t));
	if( new_proc == NULL )
	{
		kprintf("create_process: impossible de reserver la memoire pour le nouveau processus.\n");
		return -1;
	}
	
	user_stack = _PAGINATION_KERNEL_TOP + prog_size;
	sys_stack = user_stack + stack_size;
	
	len = strlen(name);
	new_proc->name = (char *) kmalloc((len+1)*sizeof(char));
	strcpy(new_proc->name, name);
	
	
	
	/* Initialisation de la pile du processus */
	/*argc = arg_build(param,(vaddr_t) &(user_stack[stack_size-1]), &argv);
	stack_ptr = (uint32_t*) argv[0];
	
	*(stack_ptr-1) = (vaddr_t) argv;
	*(stack_ptr-2) = argc;
	*(stack_ptr-3) = (vaddr_t) exit;*/
	
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
	new_proc->regs.eip = _PAGINATION_KERNEL_TOP;
	//new_proc->regs.esp = (vaddr_t)(user_stack)+stack_size-3;
	//new_proc->regs.esp = (vaddr_t)(stack_ptr-3);
	new_proc->kstack.esp0 = (vaddr_t)(&user_stack[stack_size-1]);
	new_proc->regs.ebp = new_proc->regs.esp;
	
	new_proc->kstack.ss0 = 0x10;
	//new_proc->kstack.esp0 = (vaddr_t)(sys_stack)+stack_size-1;
	new_proc->kstack.esp0 = (vaddr_t)(&sys_stack[stack_size-1]);
	
	new_proc->state = PROCSTATE_IDLE;
	
  // Initialisation des données pour la vmm
	new_proc->vm = (struct virtual_mem *) kmalloc(sizeof(struct virtual_mem));

	// Ne devrait pas utiliser kmalloc. Cf remarque suivante.
	new_proc->pd = kmalloc_one_aligned_page();
	paddr_t pd_paddr = vmm_get_page_paddr((vaddr_t) new_proc->pd);
	pagination_init_page_directory_copy_kernel_only(new_proc->pd, pd_paddr);

	// Passer l'adresse physique et non virtuelle ! Attention, il faut que ça 
	// soit contigü en mémoire physique et aligné dans un cadre...
	pagination_load_page_directory((struct page_directory_entry *) pd_paddr);
	
	init_process_vm(new_proc->vm, calculate_min_pages(prog_size + 2*stack_size));
	
	/* On copie le programme au bon endroit */
	memcpy(_PAGINATION_KERNEL_TOP, prog_addr, prog_size);

	for(i=0;i<FOPEN_MAX;i++) 
		new_proc->fd[i].used = FALSE;
		
	/* Initialisation des entrées/sorties standards */
	init_stdfiles(&new_proc->stdin, &new_proc->stdout, &new_proc->stderr);
	init_stdfd(&(new_proc->fd[0]), &(new_proc->fd[1]), &(new_proc->fd[2]));
	// Plante juste après le stdfd avec qemu lorsqu'on a déjà créé 2 process. Problème avec la mémoire ?
	proc_count++;

	add_process(new_proc);
	
	active_process = new_proc;
	if (active_process->fd[1].used) {
		focus((text_window *)(active_process->fd[1].ofd->extra_data));
	}

	return new_proc->pid;
}

int create_process(char* name, paddr_t prog, char* param, uint32_t stack_size, uint8_t ring __attribute__ ((unused)))
{
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
		return -1;
	}
	len = strlen(name);
	new_proc->name = (char *) kmalloc((len+1)*sizeof(char));
	strcpy(new_proc->name, name);
	
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
	
	/* Initialisation de la pile du processus */
	argc = arg_build(param,(vaddr_t) &(user_stack[stack_size-1]), &argv);
	stack_ptr = (uint32_t*) argv[0];
	
	*(stack_ptr-1) = (vaddr_t) argv;
	*(stack_ptr-2) = argc;
	*(stack_ptr-3) = (vaddr_t) exit;
	
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
	
  // Initialisation des données pour la vmm
	new_proc->vm = (struct virtual_mem *) kmalloc(sizeof(struct virtual_mem));

	// Ne devrait pas utiliser kmalloc. Cf remarque suivante.
	new_proc->pd = kmalloc_one_aligned_page();
	paddr_t pd_paddr = vmm_get_page_paddr((vaddr_t) new_proc->pd);
	pagination_init_page_directory_copy_kernel_only(new_proc->pd, pd_paddr);

	// Passer l'adresse physique et non virtuelle ! Attention, il faut que ça 
	// soit contigü en mémoire physique et aligné dans un cadre...
	pagination_load_page_directory((struct page_directory_entry *) pd_paddr);
	
	init_process_vm(new_proc->vm, 1);
	
	if(current_proclist_cell != NULL)
	{
		pd_paddr = vmm_get_page_paddr((vaddr_t) get_current_process()->pd);
		pagination_load_page_directory(get_current_process()->pd);
	}
	

	for(i=0;i<FOPEN_MAX;i++) 
		new_proc->fd[i].used = FALSE;
		
	/* Initialisation des entrées/sorties standards */
	init_stdfiles(&new_proc->stdin, &new_proc->stdout, &new_proc->stderr);
	init_stdfd(&(new_proc->fd[0]), &(new_proc->fd[1]), &(new_proc->fd[2]));
	// Plante juste après le stdfd avec qemu lorsqu'on a déjà créé 2 process. Problème avec la mémoire ?
	proc_count++;

	add_process(new_proc);
	
	active_process = new_proc;
	if (active_process->fd[1].used) {
		focus((text_window *)(active_process->fd[1].ofd->extra_data));
	}
	

	return new_proc->pid;
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
	if (active_process->fd[1].used) {
		focus((text_window *)(active_process->fd[1].ofd->extra_data));
	}
}

/*
 * SYSCALL
 */

void sys_exit(uint32_t ret_value __attribute__ ((unused)), uint32_t zero1 __attribute__ ((unused)), uint32_t zero2 __attribute__ ((unused)))
{
	process_t* current;
	// On cherche le processus courant:
	current = get_current_process();
	
	// On a pas forcement envie de supprimer le processus immédiatement
	current->state = PROCSTATE_TERMINATED; 
	
	//kprintf("DEBUG: exit(process %d returned %d)\n", current->pid, ret_value);

	if (current == active_process) {
		//change_active_process();
	}

}

void sys_getpid(uint32_t* pid, uint32_t zero1 __attribute__ ((unused)), uint32_t zero2 __attribute__ ((unused)))
{
	process_t* process = get_current_process();
	*pid = process->pid;
}

void sys_kill(uint32_t pid, uint32_t zero1 __attribute__ ((unused)), uint32_t zero2 __attribute__ ((unused)))
{
	process_t* process = find_process(pid);
	
	// Violent? OUI!
	if (process != NULL)
		process->state = PROCSTATE_TERMINATED;
		
	delete_process(process->pid);
}

void sys_exec(paddr_t prog, char* name, uint32_t unused __attribute__ ((unused)))
{
	int len = strlen(name);
	char * args = (char *) kmalloc((len+1)*sizeof(char));
	strcpy(args, name);;

	create_process(name, prog, args , 0x1000, 3);
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

void sys_proc(uint32_t sub_func, uint32_t param1, uint32_t param2)
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
