/**
 * @file kprocess.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2014 TacOS developers.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * @section DESCRIPTION
 *
 * Description de ce que fait le fichier
 */

#include <debug.h>
#include <gdt.h>
#include <kfcntl.h>
#include <klibc/string.h>
#include <klog.h>
#include <kmalloc.h>
#include <ksem.h>
#include <ksignal.h>
#include <ksyscall.h>
#include <pagination.h>
#include <scheduler.h>
#include <types.h>
#include <elf.h>
#include <kstat.h>
#include <kunistd.h>

#define GET_PROCESS 0
#define GET_PROCESS_LIST 1
#define FIRST_PROCESS 0
#define NEXT_PROCESS 1
#define PREV_PROCESS 2

typedef int (*main_func_type) (uint32_t, uint8_t**);

static int proc_count = -1;
static uint32_t next_pid = 0;	/* pid à donner au prochain processus créé */

static process_t* process_array[MAX_PROC];

uint32_t get_proc_count()
{
	return proc_count;
}

process_t* get_process_array(int i) {
	return process_array[i];
}

static void add_process(process_t* process)
{
	int i = 0;

	if(proc_count < MAX_PROC)
	{
		while(i<MAX_PROC && process_array[i] != NULL)
			i++;
		
		process_array[i] = process;
		proc_count++;
	}
}

process_t* find_process(int pid)
{
	process_t* proc = NULL;
	int i = 0;
	
	while(proc==NULL && i<MAX_PROC)
	{
		if(process_array[i] != NULL && process_array[i]->pid == pid)
			proc = process_array[i];
		else
			i++;
	} 
	return proc;
}

int delete_process(int pid)
{
	process_t* proc = NULL;
	int i = 0;
	
	while(proc==NULL && i<MAX_PROC)
	{
		if(process_array[i]->pid == pid)
			proc = process_array[i];
		else
			i++;
	} 
	if(proc != NULL)
	{
		kfree(proc);
		process_array[i] = NULL;
		
		proc_count--;
	}
	return 0;
}

/* Procédure permettant de construire le char** argv */
/* TODO: un schéma */
/* Valeur retournée: argc */
static int arg_build(char* string, vaddr_t base, char*** argv_ptr)
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
	str_copy_base =(char*)(base-(argc*sizeof(char*)) - 2); /* Pareil, mais on va l'utiliser en descendant dans la mémoire, et non en montant comme avec un tableau */
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
			//kprintf("%i:%s\n", ptr+1);
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

static char** envp_build(uint32_t* base, char** envp) {
	int i;
	char **renvp;
	char* str_copy_base;
	int taille_env = 0;
	while (envp[taille_env++]);

	renvp = (char**)(base - taille_env*sizeof(char*));
	str_copy_base = (char*)(renvp - 1);

	renvp[taille_env - 1] = NULL;
	// On commence par la fin pour que renvp[0] soit le plus petit.
	for (i = taille_env - 2; i >= 0; i--) {
		renvp[i] = str_copy_base - strlen(envp[i]) - 1;
		strcpy(renvp[i], envp[i]);
		str_copy_base = renvp[i];
	}

	return renvp;
}

static vaddr_t init_stack(uint32_t* base, char* args, char** envp, paddr_t return_func) {
		int argc;
		char** argv;
		uint32_t* stack_ptr;
		
		char** benvp = envp_build(base, envp);
		argc = arg_build(args, (vaddr_t)benvp[0], &argv);
		
		stack_ptr = (uint32_t*) argv[0];
		*(stack_ptr-1) = (vaddr_t) benvp;
		*(stack_ptr-2) = (vaddr_t) argv;
		*(stack_ptr-3) = argc;
		*(stack_ptr-4) = (vaddr_t) return_func;
		
		stack_ptr = stack_ptr - 4;
		
		return (vaddr_t) stack_ptr;
}

static void init_regs(regs_t* regs, vaddr_t esp, vaddr_t kesp, vaddr_t eip) {
	regs->eax = 0;
	regs->ebx = 0;
	regs->ecx = 0;
	regs->edx = 0;
	regs->esp = esp;
	regs->ebp = regs->esp;
	
	regs->cs = USER_CODE_SEGMENT;
	regs->ss = USER_STACK_SEGMENT;
	regs->ds = USER_DATA_SEGMENT;
	regs->es = 0x0;
	regs->fs = 0x0;
	regs->gs = 0x0;
	
	regs->eflags = 0;
	regs->eip = eip;
	
	regs->kss = KERNEL_STACK_SEGMENT;
	regs->kesp = kesp;
}

static process_init_data_t* dup_init_data(process_init_data_t* init_data) {
	
	process_init_data_t* dup = kmalloc(sizeof(process_init_data_t));
	
	dup->name = strdup(init_data->name);
	dup->args = strdup(init_data->args);
	
	if (init_data->envp) {
		int i;
		int taille_env = 0;
		while (init_data->envp[taille_env++]);
		dup->envp = kmalloc(taille_env * sizeof(char*));
		for (i = 0; i < taille_env - 1; i++) {
			dup->envp[i] = strdup(init_data->envp[i]);
		}
		dup->envp[taille_env - 1] = NULL;
	} else {
		dup->envp = NULL;
	}

	dup->exec_type = init_data->exec_type;
	
	dup->data = kmalloc(init_data->mem_size +1);
	memcpy(dup->data, init_data->data, init_data->mem_size);
	
	dup->mem_size = init_data->mem_size;
	dup->entry_point = init_data->entry_point;	
	dup->stack_size = init_data->stack_size;
	dup->priority = init_data->priority;
	dup->ppid = init_data->ppid;
	
	return dup;
}

static void free_init_data(process_init_data_t* init_data) {
	
	kfree(init_data->name);
	kfree(init_data->args);
	kfree(init_data->data);

	if (init_data->envp) {
		int i = 0;
		while (init_data->envp[i]) {
			kfree(init_data->envp[i]);
			i++;
		}
		kfree(init_data->envp);
	}

	kfree(init_data);
}

static process_t* create_process_elf(process_init_data_t* init_data)
{
	uint32_t *sys_stack, *user_stack;
	process_init_data_t* init_data_dup;
	process_t* new_proc;
	
	uint32_t* stack_ptr;
		
	int i;
	
	init_data_dup = dup_init_data(init_data);
	
	new_proc = kmalloc(sizeof(process_t));
	if( new_proc == NULL )
	{
		kerr("impossible de reserver la memoire pour le nouveau processus.");
		return NULL;
	}
	
	new_proc->name = strdup(init_data_dup->name);
	
	/* Initialisation de la kernel stack */
	sys_stack = kmalloc(init_data_dup->stack_size*sizeof(uint32_t));
	if( new_proc == NULL )
	{
		kerr("impossible de reserver la memoire pour la pile systeme.");
		return NULL;
	}
	
	// Sémaphore pour le wait
	new_proc->sem_wait = ksemget(SEM_NEW, SEM_CREATE);
	int val = 0;
	ksemctl(new_proc->sem_wait, SEM_SET, &val);

	new_proc->ppid = init_data_dup->ppid;
	
	// Initialisation des données pour la vmm
	new_proc->vm = (struct virtual_mem *) kmalloc(sizeof(struct virtual_mem));

	// Ne devrait pas utiliser kmalloc. Cf remarque suivante.
	new_proc->pd = kmalloc_one_aligned_page();
	paddr_t pd_paddr = vmm_get_page_paddr((vaddr_t) new_proc->pd);
	pagination_init_page_directory_copy_kernel_only(new_proc->pd, pd_paddr);
	
	/* Sélection du pid du process */
	new_proc->pid = next_pid;
	next_pid++;

	new_proc->ctrl_tty = NULL;
	
	/* ZONE CRITIQUE */
	asm("cli");

		// Passer l'adresse physique et non virtuelle ! Attention, il faut que ça 
		// soit contigü en mémoire physique et aligné dans un cadre...
		pagination_load_page_directory((struct page_directory_entry *) pd_paddr);
		
		init_process_vm(new_proc->vm, calculate_min_pages(init_data_dup->mem_size + (init_data_dup->stack_size)*sizeof(uint32_t)));
		
		/* Copie du programme au bon endroit */
		memcpy((void*)USER_PROCESS_BASE, (void*)init_data_dup->data, init_data_dup->mem_size);
		
		/* Initialisation de la pile utilisateur */
		user_stack = (uint32_t*) (USER_PROCESS_BASE + init_data_dup->mem_size + init_data_dup->stack_size-1);
		stack_ptr = (uint32_t*) init_stack(user_stack, init_data_dup->args, init_data_dup->envp, (paddr_t)sys_exit);
		
		/* TODO : Ajouter (ici ?) le passage de l'environnement utilisateur */

		/* Remet le repertoire de page du process courant (le père donc) */
		if(proc_count > 0)
		{
			pd_paddr = vmm_get_page_paddr((vaddr_t) get_current_process()->pd);
			pagination_load_page_directory((struct page_directory_entry *) pd_paddr);
		}
		
		for (i = 0; i < FOPEN_MAX; i++) { 
			new_proc->fd[i] = NULL;
		}

		/* Initialisation des entrées/sorties standards */
		init_stdfd(new_proc);
		
	/* FIN ZONE CRITIQUE */
	asm("sti");
	
	/* Initialisation des registres */
	init_regs(&(new_proc->regs), (vaddr_t)stack_ptr, (vaddr_t)(&sys_stack[init_data_dup->stack_size-1]), init_data_dup->entry_point);
	
	/* Initialisation des compteurs de temps CPU */
	new_proc->user_time = 0;
	new_proc->sys_time = 0;
	new_proc->current_sample = 0;
	new_proc->last_sample = 0;
	

	/* On attend son premier ordonnancement pour le passer en RUNNING, donc pour le moment on le laisse IDLE */
	new_proc->state = PROCSTATE_IDLE;
	
	/* Initilisation des masques de signal */
	new_proc->signal_data.mask = 0;
	new_proc->signal_data.pending_set = 0;
	for (i = 0; i < NSIG; i++) {
		new_proc->signal_data.handlers[i] = NULL;
	}
	
	/* Création de la table des symboles */
	new_proc->symtable = load_symtable(init_data->file);

	add_process(new_proc);

	/* Plantait, mais ne plante plus... */
	free_init_data(init_data_dup);
	
	return new_proc;
}

int create_kprocess(char* _name, void* entry_point, uint32_t _stack_size)
{
	process_init_data_t init_data;

	init_data.name = _name;
	init_data.data = entry_point;
	init_data.stack_size = _stack_size;
	
	process_t* proc = create_process(&init_data);
	/*XXX verifier que le process est bien cree */
	scheduler_add_process(proc);
	return proc->pid;
}

process_t* create_process(process_init_data_t* init_data) {
	char* name = init_data->name;
	paddr_t prog = (paddr_t) init_data->data;
	char* param = "";
	uint32_t stack_size = init_data->stack_size;

	uint32_t *sys_stack, *user_stack;
	process_t* new_proc;
	
	char** argv;
	int argc;
	uint32_t* stack_ptr;
	
	int i;
	
	new_proc = kmalloc(sizeof(process_t));
	if (new_proc == NULL)
	{
		kerr("impossible de reserver la memoire pour le nouveau processus.");
		return NULL;
	}
	new_proc->name = strdup(name);
	
	sys_stack = kmalloc(0x100000);
	if (sys_stack == NULL)
	{
		kerr("impossible de reserver la memoire pour la pile systeme.");
		return NULL;
	}
	
	user_stack = kmalloc(stack_size*sizeof(uint32_t));
	if (user_stack == NULL)
	{
		kerr("impossible de reserver la memoire pour la pile utilisateur.");
		return NULL;
	}
	
	/* Initialisation de la pile du processus */
	argc = arg_build(param,(vaddr_t) &(user_stack[stack_size-1]), &argv);
	stack_ptr = (uint32_t*) argv[0];
	
	*(stack_ptr-1) = (vaddr_t) NULL;
	*(stack_ptr-2) = (vaddr_t) argv;
	*(stack_ptr-3) = argc;
	*(stack_ptr-4) = (vaddr_t) sys_exit;
	
	new_proc->ppid = 0;

	new_proc->sem_wait = ksemget(SEM_NEW, SEM_CREATE);
	int val = 0;
	ksemctl(new_proc->sem_wait, SEM_SET, &val);
	
	new_proc->user_time = 0;
	new_proc->sys_time = 0;
	new_proc->current_sample = 0;
	new_proc->last_sample = 0;
	
	new_proc->pid = next_pid;
	new_proc->regs.eax = 0;
	new_proc->regs.ebx = 0;
	new_proc->regs.ecx = 0;
	new_proc->regs.edx = 0;

	new_proc->regs.esi = 0;
	new_proc->regs.edi = 0;

	new_proc->regs.cs = 0x8;
	new_proc->regs.ds = 0x10;
	new_proc->regs.ss = 0x10;

	new_proc->regs.es = 0;
	new_proc->regs.fs = 0;
	new_proc->regs.gs = 0;
	new_proc->regs.cr3 = 0;
	
	new_proc->regs.eflags = 0;
	new_proc->regs.eip = prog;
	new_proc->regs.esp = (vaddr_t)(stack_ptr-4);
	new_proc->regs.ebp = new_proc->regs.esp;
	
	new_proc->regs.kss = 0x10;
	new_proc->regs.kesp = (vaddr_t)(&sys_stack[stack_size-1]);
	
	new_proc->state = PROCSTATE_IDLE;
	
	new_proc->ctrl_tty = NULL;

	/* Initialisation des signaux */
	for(i=0; i<NSIG; i++)
		new_proc->signal_data.handlers[i] = NULL;
	
	new_proc->signal_data.mask = 0;
	new_proc->signal_data.pending_set = 0;
	
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
	
	if(proc_count > 0)
	{
		pd_paddr = vmm_get_page_paddr((vaddr_t) get_current_process()->pd);
		pagination_load_page_directory((struct page_directory_entry *) pd_paddr);
	}

	for (i = 0; i < FOPEN_MAX; i++) {
		new_proc->fd[i] = NULL;
	}

	/* Initialisation des entrées/sorties standards */
	init_stdfd(new_proc);
	// Plante juste après le stdfd avec qemu lorsqu'on a déjà créé 2 process. Problème avec la mémoire ?
	next_pid++;
	
	add_process(new_proc);
	
	/* FIN ZONE CRITIQUE */
	asm("sti");	
	
	return new_proc;	
}

void sample_CPU_usage()
{
	int i = 0;
	for(i=0; i<MAX_PROC; i++)
	{
		if(process_array[i] != NULL)
		{
			process_array[i]->last_sample = process_array[i]->current_sample;
			process_array[i]->current_sample = 0;
		}
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
	
	close_all_fd();

	//kprintf("DEBUG: exit(process %d returned %d)\n", current->pid, ret_value);
	// On a pas forcement envie de supprimer le processus immédiatement
	ksemctl(current->sem_wait, SEM_DEL, NULL);
	sys_kill(current->ppid, SIGCHLD, NULL);
	current->state = PROCSTATE_TERMINATED;
}

SYSCALL_HANDLER1(sys_getpid, uint32_t* pid)
{
	process_t* process =get_current_process();
	*pid = process->pid;
}

SYSCALL_HANDLER1(sys_getppid, uint32_t* ppid)
{
	process_t* process =get_current_process();
	*ppid = process->ppid;
}

static int sys_exec2(process_init_data_t* init_data)
{
	process_t *process;
	if(init_data->exec_type == EXEC_KERNEL) {
	//	process = create_process(init_data);
		kerr("If you reached this message, you're a nasty boy.");
	} else {
		process = create_process_elf(init_data);
	}
	scheduler_add_process(process);
	return process->pid;
}

SYSCALL_HANDLER3(sys_exec, char *cmdline, char **environ, int *retval)
{
	process_t* process = get_current_process();
	int pid = process->pid;

	char* execpath = strdup(cmdline);

	int ret = -1;
	int offset = 0;
	
	while(execpath[offset] != ' ' && execpath[offset] != '\0')
		offset++;
	
	execpath[offset] = '\0';
	
	struct stat buf;
	sys_stat(execpath, &buf, &ret);

	if (!S_ISREG(buf.st_mode) || !(S_IXUSR & buf.st_mode)) {
		*retval = -1;
		return;
	}

	int fd;
	sys_open(&fd, execpath, O_RDONLY);
	
	process_init_data_t init_data;
	
	ret = -1;
	if(fd != -1)
	{
		init_data.name	= execpath;
		init_data.stack_size = 0x1000;
		init_data.priority = 0;
		
		init_data.args = cmdline;
		init_data.envp = environ;
		
		init_data.mem_size = elf_size(fd);
		init_data.data = kmalloc(init_data.mem_size);
		memset(init_data.data, 0, init_data.mem_size);
		init_data.entry_point = load_elf(fd, init_data.data);
		
		init_data.file = load_elf_file(fd);

		init_data.ppid = pid;
		init_data.exec_type = EXEC_ELF;

		ret = sys_exec2(&init_data);

		kfree(init_data.data);
		uint32_t r;
		sys_close(fd, &r, 0);
	}
	kfree(execpath);
	
	*retval = ret;
}

process_t* sys_proc_list(uint32_t action)
{
	static int index = -1;
	process_t* ret = NULL;
	switch(action)
	{
		case FIRST_PROCESS:
			index = 0;	
			break;
		case NEXT_PROCESS:
			if(index<MAX_PROC-1)
			{
				do {
					index++;
				}while(index<(MAX_PROC-1) && process_array[index] == NULL);
			}
			break;
		case PREV_PROCESS:
			if(index>0)
			{
				do {
					index--;
				}while(index>0 && process_array[index] == NULL);
			}
			break;
		default:
			kerr("invalid action (0x%x).", action);
	}
	
	if( index >= 0 )
		ret = process_array[index];
		
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
			kerr("invalid syscall (0x%x).\n", sub_func);
	}
}

SYSCALL_HANDLER1(sys_waitpid, int pid) {
	if (pid <= 0) {
		klog("waitpid <= 0 non supporté pour l'instant.");
	} else {
		process_t* proc = find_process(pid);
		if (proc) {
			if (proc->state != PROCSTATE_TERMINATED) {
				ksemP(proc->sem_wait);
			}
		} else {
			klog("waitpid sur un process non trouvé.");
		}
	}
}
