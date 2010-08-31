
/**
 * @file scheduler.c
 */

#include <stdio.h>
#include <types.h>
#include <kprocess.h>
#include <scheduler.h>
#include <events.h>
#include <clock.h>
#include <ioports.h>
#include <gdt.h>
#include <i8259.h>
#include <syscall.h>
#include <debug.h>
#include <vmm.h>
#include <kmalloc.h>
#include <string.h>

#define USER_PROCESS 0
#define KERNEL_PROCESS 1

static uint32_t quantum;	/* Quantum de temps alloué aux process */
static int event_id = 0;	/* Identifiant de l'évenement schedule */
static int sample_counter;	/* Compteur du nombre d'échantillonnage pour l'évaluation de l'usage CPU */

static process_t* idle_process = NULL;
static proclist_cell idle_proclist_cell;

void idle()
{
	while(1)
		halt();
}


/* Effectue le changement de contexte proprement dit */
void process_switch(int mode, process_t* current)
{
	uint32_t esp, eflags;
	uint16_t kss, ss, cs;
	
	/* Récupère l'adresse physique de répertoire de page du processus */
	paddr_t pd_paddr = vmm_get_page_paddr((vaddr_t) current->pd);
	
	/* Mise à jour des piles de la TSS */
	get_default_tss()->esp0	=	current->kstack.esp0;
	get_default_tss()->ss0	=	current->kstack.ss0;
	
	ss = current->regs.ss;
	cs = current->regs.cs;
	//esp0 = current-e>regs.esp;
	eflags = (current->regs.eflags | 0x200) & 0xFFFFBFFF; // Flags permettant le changemement de contexte
	exec_sighandler(current);
	if(mode == USER_PROCESS)
	{
		kss = current->kstack.ss0;
		esp = current->kstack.esp0;
	}
	else
	{
		kss = current->regs.ss;
		esp = current->regs.esp;
	}
	asm(
			"mov %0, %%ss;"
			"mov %1, %%esp;"
			"cmp %[KPROC], %[mode];" // if(mode != KERNEL_PROC)
			"je else;"
			"push %2;"
			"push %3;"
			"else:"
			"push %4;"
			"push %5;"
			"push %6;"
			::        
			"m" (kss), 		
			"m" (esp),
			"m" (ss), 					
			"m" (current->regs.esp), 	
			"m" (eflags), 				
			"m" (cs), 					
			"m" (current->regs.eip),
			[KPROC] "i"(KERNEL_PROCESS),
			[mode]	"g"(mode) 	
       );
	/* On push ensuite les registres de la tache à lancer */
	
	asm(
			"push %0\n\t"
			"push %1\n\t"
			"push %2\n\t"
			"push %3\n\t"
			"push %4\n\t"
			"push %5\n\t"
			::"m" (current->regs.eax),
			"m" (current->regs.ecx),
			"m" (current->regs.edx),
			"m" (current->regs.ebx),
			"m" (current->regs.ebp),
			"m" (current->regs.esi)
	);
	asm(
			"push %0\n\t"
			"push %1\n\t"
			"push %2\n\t"
			"push %3\n\t"
			"push %4\n\t"
			::
			"m" (current->regs.edi),	
			"m" (current->regs.ds),		
			"m" (current->regs.es),		
			"m" (current->regs.fs),		
			"m" (current->regs.gs)		
	);
	
	outb(0x20, 0x20);

	/*	pagination_load_page_directory(pd_paddr); */
	asm volatile("mov %0, %%cr3":: "b"(pd_paddr));

	asm(
		/*On dépile les registres pour les mettre en place */
		"pop %gs\n\t"
		"pop %fs\n\t"
		"pop %es\n\t"
		"pop %ds\n\t"
		"pop %edi\n\t"
		"pop %esi\n\t"
		"pop %ebp\n\t"
		"pop %ebx\n\t"
		"pop %edx\n\t"
		"pop %ecx\n\t"
		"pop %eax\n\t"
		
		/* Et on switch! (enfin! >_<) */
		"iret\n\t"
	);
}

void* schedule(void* data __attribute__ ((unused)))
{
	uint32_t* stack_ptr;
	uint32_t compteur;

	process_t* current = get_current_process();
	
	/* On récupère le contexte du processus actuel uniquement si il a déja été lancé */
	if(current->state == PROCSTATE_RUNNING || current->state == PROCSTATE_WAITING)
	{	
		/* On récupere un pointeur de pile pour acceder aux registres empilés */
		asm("mov (%%ebp), %%eax; mov %%eax, %0" : "=m" (stack_ptr) : );
		
		/* On met le contexte dans la structure "process"
		 * (on peut difficilement faire ça dans une autre fonction, sinon il 
		 * faudrait calculer l'offset résultant dans la pile, donc c'est 
		 * hardcodé, et c'est bien comme ça.)  
		 */
		current->regs.eflags = stack_ptr[17];
		current->regs.cs  = stack_ptr[16];
		current->regs.eip = stack_ptr[15];
		current->regs.eax = stack_ptr[14];
		current->regs.ecx = stack_ptr[13];
		current->regs.edx = stack_ptr[12];
		current->regs.ebx = stack_ptr[11];
		/* ->esp kernel, on saute */
		current->regs.ebp = stack_ptr[9];
		current->regs.esi = stack_ptr[8];
		current->regs.edi = stack_ptr[7];
		current->regs.fs = stack_ptr[6];
		current->regs.gs = stack_ptr[5];
		current->regs.ds = stack_ptr[4];
		current->regs.es = stack_ptr[3];
		
		/* Si on ordonnance une tache en cours d'appel systeme.. */
		if(current->regs.cs == 0x8)
		{
			current->regs.ss = get_default_tss()->ss0;
			current->regs.esp = stack_ptr[10] + 12;	/* Valeur hardcodée, je cherche encore un moyen d'éviter ça... */
		}
		else
		{
			current->regs.ss = stack_ptr[19];
			current->regs.esp = stack_ptr[18];
		}
		
		/* Sauver la TSS */
		current->kstack.esp0 = get_default_tss()->esp0;
		current->kstack.ss0  = get_default_tss()->ss0;
		
		current->user_time += quantum;
	}
	
	/* On recupere le prochain processus à executer.
	 * TODO: Dans l'idéal, on devrait ici faire appel à un scheduler, 
	 * qui aurait pour rôle de choisir le processus celon une politique spécifique */
	compteur = 0;
	do
	{
		compteur++;
		current = get_next_process();
	}while((current->state == PROCSTATE_TERMINATED || current->state == PROCSTATE_WAITING) && compteur < get_proc_count());
	
	/* Evaluation de l'usage du CPU */
	current->current_sample++;
	sample_counter++;
	if(sample_counter >= CPU_USAGE_SAMPLE_RATE)
	{
		sample_CPU_usage();
		sample_counter = 0;
	}
	
	
	if(current == NULL || current->state == PROCSTATE_WAITING || current->state == PROCSTATE_TERMINATED) 
	{
		/* Si on a rien à faire, on passe dans le processus idle */
		proclist_cell* cell = get_current_proclist_cell();
		idle_proclist_cell.next = cell;
		set_current_proclist_cell(&idle_proclist_cell);	
		current = idle_proclist_cell.process;
	}

	/* Si le processus courant n'a pas encore commencé son exécution, on le lance */
	if(current->state == PROCSTATE_IDLE)
	{
		current->state = PROCSTATE_RUNNING;
	}

	/* Mise en place de l'interruption sur le quantum de temps */
	event_id = add_event(schedule,NULL,quantum*1000);	

	/* On réaffecte à la main stdin, stdout et stderr. TEMPORAIRE ! Il faudrait que stdin, stdout et stderr soient tjs à la même adresse pour chaque processus... */
	stdin = current->stdin;
	stdout = current->stdout;
	stderr = current->stderr;

	/* Changer le contexte:*/
	if(current->regs.cs == 0x8)
		process_switch(KERNEL_PROCESS, current);
	else
		process_switch(USER_PROCESS, current);
		

	return NULL;
}

void init_scheduler(int Q)
{
	quantum = Q;
	
	idle_process = create_process("idle", idle, "idle", 0x100, 3);
	idle_process->state = PROCSTATE_IDLE;
	
	idle_proclist_cell.process = idle_process;
	idle_proclist_cell.prev = NULL;
	idle_proclist_cell.next = NULL;
}

void stop_scheduler()
{
	del_event(event_id);
}

void start_scheduler()
{
	event_id = add_event(schedule,NULL,quantum*1000);
}

void* sleep_callback( void* data )
{
	process_t* proc = (process_t*) data;
	proc->state = PROCSTATE_RUNNING;
	return NULL;
}

void halt()
{
	syscall(SYS_HLT, NULL, NULL, NULL);
}

void sys_hlt(uint32_t unused1 __attribute__ ((unused)), uint32_t unused2 __attribute__ ((unused)), uint32_t unused3 __attribute__ ((unused)))
{
	asm("hlt");
}

void sys_sleep( uint32_t delay,uint32_t unused2 __attribute__ ((unused)), uint32_t unused3 __attribute__ ((unused)))
{
	/* Désactivation de l'ordonnanceur */
	stop_scheduler();
	
	process_t* process = get_current_process();

	/* Passage du processus en waiting */
	process->state = PROCSTATE_WAITING;

	/* Adjout de l'évènement de fin de sleep */
	add_event(sleep_callback,(void*)process,delay);
	
	/* Scheduling immédiat */
	start_scheduler();
	
	
	while(process->state == PROCSTATE_WAITING);
	//dummy1();
		
	return NULL;
}
