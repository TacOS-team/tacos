/**
 * @file kprocess.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2015 TacOS developers.
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
#include <vfs.h>
#include <memory.h>
#include <vmm.h>

#define GET_PROCESS 0
#define GET_PROCESS_LIST 1
#define FIRST_PROCESS 0
#define NEXT_PROCESS 1
#define PREV_PROCESS 2

typedef int (*main_func_type) (uint32_t, uint8_t**);

static int proc_count = -1;

static process_t* process_array[MAX_PROC];

unsigned long long user_time;
unsigned long long sys_time;
unsigned long long idle_time;

paddr_t reserve_page_frame(process_t* process) {
	paddr_t addr = memory_reserve_page_frame();
	if (addr) {
		if (process) {
			struct physical_page_descr *el = kmalloc(sizeof(struct physical_page_descr));
			el->addr = addr;
			el->next = process->reserved_pages;
			process->reserved_pages = el;
		}
		return addr;
	}
	return 0;
}

void release_page_frames(process_t* process) {
	struct physical_page_descr *aux = process->reserved_pages;
	while (aux) {
		memory_free_page_frame(aux->addr);
		aux = aux->next;
	}
}

uint32_t get_proc_count()
{
	return proc_count;
}

process_t* get_process_array(int i) {
	return process_array[i];
}

static int add_process(process_t* process)
{
	if (proc_count == MAX_PROC) {
		return -1;
	}

	static uint32_t next_pid = 0;	/* pid à donner au prochain processus créé */

	while (process_array[next_pid] != NULL) {
		next_pid++;
		if (next_pid == MAX_PROC) {
			next_pid = 0;
		}
	}

	process_array[next_pid] = process;
	process->pid = next_pid;
	proc_count++;

	return next_pid;
}

process_t* find_process(int pid)
{
	return process_array[pid];
}

int delete_process(int pid)
{
	process_t* proc = process_array[pid];
	if (proc != NULL) {
		kfree(proc);
		process_array[pid] = NULL;
		proc_count--;
		return 0;
	}
	return -1;
}

/* Procédure permettant de construire le char** argv */
/* TODO: un schéma */
/* Valeur retournée: argc */
static int arg_build(char* string, vaddr_t base, char*** argv_ptr)
{
	int argc = 0;
	int len = 0;
	int i;
	
	char* ptr = string;
	char** argv;
	char* str_copy_base;
	
	/* Première passe: on compte argc et le nombre total de caractères de la chaine, et on remplace les espaces par des 0*/
	int word = 0;
	while(*ptr != '\0')
	{
		/* Si on a un espace on a une sous-chaine, et on incrément argc */
		if(*ptr == ' ')
		{
			*ptr = '\0';
			if (word) {
				argc++;
			}
			word = 0;
		} else {
			word = 1;
		}
		
		ptr++;
		len++;
	}
	argc += word;
	argv = (char**) (base-(argc*sizeof(char*))); /* Base moins les argc cases pour les argv[x] */ 
	str_copy_base =(char*)(base-(argc*sizeof(char*)) - 2); /* Pareil, mais on va l'utiliser en descendant dans la mémoire, et non en montant comme avec un tableau */
	i = argc-1;
	
	*str_copy_base = '\0';
	ptr--;
	len--;
	
	word = 0;
	while(len >= 0)
	{
		/* Si on trouve '\0', c'est qu'on a fini de parcourir un mot, et on fait donc pointer l'argv comme il faut */
		if(*ptr == '\0') {
			if (word) {
				argv[i] = str_copy_base+1;
				//kprintf("%d:%s\n", i, argv[i]);
				i--;
			}
			word = 0;
		} else {
			word = 1;
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

static void init_regs(regs_t* regs, vaddr_t esp, vaddr_t kesp, vaddr_t eip, uint8_t isKernel) {
	regs->eax = 0;
	regs->ebx = 0;
	regs->ecx = 0;
	regs->edx = 0;
	regs->esp = esp;
	regs->ebp = regs->esp;
	regs->esi = 0;
	regs->edi = 0;

	if(isKernel) {
		regs->cs = KERNEL_CODE_SEGMENT;
		regs->ds = KERNEL_DATA_SEGMENT;
		regs->ss = KERNEL_DATA_SEGMENT;
		regs->es = KERNEL_DATA_SEGMENT;
		regs->fs = KERNEL_DATA_SEGMENT;
		regs->gs = KERNEL_DATA_SEGMENT;

	} else {
		regs->cs = USER_CODE_SEGMENT;
		regs->ss = USER_STACK_SEGMENT;
		regs->ds = USER_DATA_SEGMENT;
		regs->es = USER_DATA_SEGMENT;
		regs->fs = USER_DATA_SEGMENT;
		regs->gs = USER_DATA_SEGMENT;
	}
		
	regs->eflags = 0;
	regs->eip = eip;
	regs->cr3 = 0;

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

process_t* create_process(process_init_data_t* init_data, uint8_t isKernel) {
	process_t* new_proc;
	uint32_t *sys_stack, *user_stack;
	uint32_t* stack_ptr;
	int i;

	/* Création  de la structure process_t */
	new_proc = kmalloc(sizeof(process_t));
	if (new_proc == NULL)
	{
		kerr("impossible de reserver la memoire pour le nouveau processus.");
		return NULL;
	}

	/* Creation de la kernel stack */
	sys_stack = kmalloc(init_data->stack_size * sizeof(uint32_t));
	if( sys_stack == NULL )
	{
		kerr("impossible de reserver la memoire pour la pile systeme.");
		kfree(new_proc);
		return NULL;
	}

	if(isKernel){
		/* Création de la main stack pour un process kernel.
 		 * Un l'alloue à part uniquement dans ce cas, car pour un process "classique" la pile est allouée en même temps que le reste
 		 */
		user_stack = kmalloc(init_data->stack_size * sizeof(uint32_t));
		if (user_stack == NULL)
		{
			kerr("impossible de reserver la memoire pour la pile utilisateur.");
			kfree(new_proc);
			kfree(sys_stack);
			return NULL;
		}
	}

	new_proc->name = strdup(init_data->name);
	new_proc->cwd = NULL;

	/* Sémaphore pour le wait */
	new_proc->sem_wait = ksemget(SEM_NEW, SEM_CREATE);
	new_proc->sem_wait_child = ksemget(SEM_NEW, SEM_CREATE);
	int val = 0;
	ksemctl(new_proc->sem_wait, SEM_SET, &val);
	ksemctl(new_proc->sem_wait_child, SEM_SET, &val);

	new_proc->nb_children = 0;
	new_proc->ppid = init_data->ppid;

	/* Initialisation des données pour la vmm */
	new_proc->vm = kmalloc(sizeof(struct virtual_mem));
	new_proc->list_regions = NULL;
	new_proc->reserved_pages = NULL;
	new_proc->pd = kmalloc_one_aligned_page(); // XXX Ne devrait pas utiliser kmalloc. Cf remarque suivante.

	paddr_t pd_paddr = vmm_get_page_paddr((vaddr_t) new_proc->pd);
	pagination_init_page_directory_copy_kernel_only(new_proc->pd, pd_paddr);

	new_proc->ctrl_tty = NULL;

	/* Initilisation des masques de signal */
	new_proc->signal_data.mask = 0;
	new_proc->signal_data.pending_set = 0;
	for (i = 0; i < NSIG; i++) {
		new_proc->signal_data.handlers[i] = NULL;
	}

	/* Initialisation des compteurs de temps CPU */
	new_proc->user_time = 0;
	new_proc->sys_time = 0;

	/* On attend son premier ordonnancement pour le passer en RUNNING, donc pour le moment on le laisse IDLE */
	new_proc->state = PROCSTATE_IDLE;

	/* ZONE CRITIQUE */
	asm("cli");
		// Passer l'adresse physique et non virtuelle ! Attention, il faut que ça 
		// soit contigü en mémoire physique et aligné dans un cadre...
		pagination_load_page_directory((struct page_directory_entry *) pd_paddr);
		if(isKernel)
			init_process_vm(new_proc, new_proc->vm, 1);
		else
			init_process_vm(new_proc, new_proc->vm, calculate_min_pages(init_data->mem_size));
		
		if(!isKernel) {
			int stack_pages = init_data->stack_size / PAGE_SIZE;

		
			// Passer l'adresse physique et non virtuelle ! Attention, il faut que ça 
			// soit contigü en mémoire physique et aligné dans un cadre...
			pagination_load_page_directory((struct page_directory_entry *) pd_paddr);

			// Allocation stack (attention à ne pas dépasser 8 Mio en l'état)
			for(i = 1; i <= stack_pages; i++)
				map(reserve_page_frame(new_proc), USER_PROCESS_STACK - i*PAGE_SIZE, 1, 1);

			/* Copie du programme au bon endroit */
			memcpy((void*)USER_PROCESS_BASE, (void*)init_data->data, init_data->mem_size);

			/* Initialisation de la pile utilisateur */
			user_stack = (uint32_t*) USER_PROCESS_STACK - 4;
			
		}

		if(isKernel) {
			/* Initialisation de la pile du processus */
			char** argv;
			int argc;
	
			argc = arg_build("",(vaddr_t) &(user_stack[init_data->stack_size-1]), &argv);
			stack_ptr = (uint32_t*) argv[0];
	
			*(stack_ptr-1) = (vaddr_t) NULL;
			*(stack_ptr-2) = (vaddr_t) argv;
			*(stack_ptr-3) = argc;
			*(stack_ptr-4) = (vaddr_t) sys_exit;
		} else {
			stack_ptr = (uint32_t*) init_stack(user_stack, init_data->args, init_data->envp, (paddr_t)sys_exit);
		}

		
		/* Remet le repertoire de page du process courant (le père donc) */
		if(proc_count > 0)
		{
			pd_paddr = vmm_get_page_paddr((vaddr_t) get_current_process()->pd);
			pagination_load_page_directory((struct page_directory_entry *) pd_paddr);
		}

		memset(new_proc->fd, 0, FOPEN_MAX * sizeof(open_file_descriptor*));
	
		add_process(new_proc);

		/* Initialisation des entrées/sorties standards */
		init_stdfd(new_proc);

	/* FIN ZONE CRITIQUE */
	asm("sti");

	/* Initialisation des registres */
	if(isKernel) {
		init_regs(&(new_proc->regs), 
			  (vaddr_t)stack_ptr-4, 
			  (vaddr_t)(&sys_stack[init_data->stack_size-1]), 
			  (paddr_t) init_data->data, 
			  isKernel);
	
	} else {
		init_regs(&(new_proc->regs), 
			  (vaddr_t)stack_ptr, 
			  (vaddr_t)(&sys_stack[init_data->stack_size-1]), 
			  init_data->entry_point, 
			  isKernel);
	}

	/* XXX: Création de la table des symboles, il faut la dupliquer dans la structure sinon ça plante */
	//new_proc->symtable = load_symtable(init_data->file);

	return new_proc;
}



int create_kprocess(char* _name, void* entry_point, uint32_t _stack_size)
{
	process_init_data_t init_data;

	init_data.name = _name;
	init_data.data = entry_point;
	init_data.stack_size = _stack_size;
	init_data.ppid = 0; /* XXX Mettre le bon ppid? */
	
	process_t* proc = create_process(&init_data, 1);
	if (proc) {
		scheduler_add_process(proc);
		return proc->pid;
	} else {
		return -1;
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
	//sys_kill(current->ppid, SIGCHLD, NULL);
	current->state = PROCSTATE_TERMINATED;

	process_t* parent = find_process(current->ppid);
	ksemV(parent->sem_wait_child);

	// Maj PPID des fils
	int i;
	for (i = 0; i < current->nb_children; i++) {
		current->children[i]->ppid = current->ppid;
	}

	release_page_frames(current);
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
	if(init_data->exec_type == EXEC_KERNEL) {
	//	process = create_process(init_data);
		kerr("If you reached this message, you're a nasty boy.");
		return -1;
	}

	process_t *process;
	process_init_data_t* init_data_dup = dup_init_data(init_data);

	process = create_process(init_data_dup, 0);

	free_init_data(init_data);

	if (process->ppid) {
		process_t *parent = find_process(process->ppid);
		if (parent->cwd) {
			process->cwd = strdup(parent->cwd);
		} else {
			process->cwd = strdup("/");
		}
		parent->children[parent->nb_children++] = process;
	}
	scheduler_add_process(process);
	return process->pid;
}

SYSCALL_HANDLER3(sys_exec, char *cmdline, char **environ, int *retval)
{
	process_t* process = get_current_process();
	int pid = process->pid;

	// execpath ne contient que la partie avant l'éventuel premier espace.
	char* spc = strchrnul(cmdline, ' ');
	char* execpath = kmalloc(spc - cmdline + 1);
	strncpy(execpath, cmdline, spc - cmdline);
	execpath[spc - cmdline] = '\0';

	struct stat buf;
	int ret = vfs_stat(execpath, &buf, 1);

	if (!S_ISREG(buf.st_mode) || !(S_IXUSR & buf.st_mode)) {
		*retval = -1;
		kfree(execpath);
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
	if (pid == -1) {
		process_t* current = get_current_process();
		ksemP(current->sem_wait_child);
	} else if (pid > 0) {
		process_t* proc = find_process(pid);
		if (proc) {
			if (proc->state != PROCSTATE_TERMINATED) {
				ksemP(proc->sem_wait);
			}
		} else {
			klog("waitpid sur un process non trouvé.");
		}
	} else {
		klog("waitpid <= 0 non supporté pour l'instant.");
	}
}
