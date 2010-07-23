/**
 * @file scheduler.c
 */

#include <stdio.h>
#include <types.h>
#include <process.h>
#include <scheduler.h>
#include <events.h>
#include <clock.h>
#include <ioports.h>
#include <gdt.h>
#include <i8259.h>
#include <syscall.h>
#include <debug.h>
#include <kmalloc.h>
#include <string.h>

#define USER_PROCESS 0
#define KERNEL_PROCESS 1

process_t* idle_process;
static uint32_t quantum;						// Quantum de temps alloué aux process
static int event_id = 0;


void process_switch(int mode, process_t* current)
{
	uint32_t esp, eflags;
	uint16_t kss, ss, cs;
	
	get_default_tss()->esp0	=	current->kstack.esp0;
	get_default_tss()->ss0	=	current->kstack.ss0;
	
	ss = current->regs.ss;
	cs = current->regs.cs;
	//esp0 = current-e>regs.esp;
	eflags = (current->regs.eflags | 0x200) & 0xFFFFBFFF; // Flags permettant le changemement de contexte
		
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
	
	asm(
        /* La il faudrait changer le répertoire de page */
        
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

static void* schedule(void* data __attribute__ ((unused)))
{
	uint32_t* stack_ptr;
   uint32_t compteur;

   process_t* current = get_current_process();
	
	// On récupère le contexte du processus actuel uniquement si il a déja été lancé
	if(current->state == PROCSTATE_RUNNING)
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
		//->esp kernel, on saute
		current->regs.ebp = stack_ptr[9];
		current->regs.esi = stack_ptr[8];
		current->regs.edi = stack_ptr[7];
		current->regs.fs = stack_ptr[6];
		current->regs.gs = stack_ptr[5];
		current->regs.ds = stack_ptr[4];
		current->regs.es = stack_ptr[3];
		
		// Si on ordonnance une tache en cours d'appel systeme..
		if(current->regs.cs == 0x8)
		{
			current->regs.ss = get_default_tss()->ss0;
			current->regs.esp = stack_ptr[10] + 12;
		}
		else
		{
			current->regs.ss = stack_ptr[19];
			current->regs.esp = stack_ptr[18];
		}
		
		// Sauver la TSS
		current->kstack.esp0 = get_default_tss()->esp0;
		current->kstack.ss0  = get_default_tss()->ss0;
		
		current->user_time += quantum;
	}
	
	// On recupere le prochain processus à executer	
	compteur = 0;
	do
	{
		compteur++;
		current = get_next_process();
	}while((current->state == PROCSTATE_TERMINATED || current->state == PROCSTATE_WAITING) && compteur < get_proc_count());
	
	// Si on a aucun processus en IDLE/WAITING/RUNNING, il n'y a aucune chance pour qu'un processus arrive spontanement
	// Donc on arrete le scheduler
	if(current == NULL || current->state == PROCSTATE_TERMINATED) 
	{
		// Mise en place de l'interruption sur le quantum de temps
	    
	    add_event(schedule,NULL,quantum);	
	    i8254_init(1000/*TIMER_FREQ*/);
		kprintf("Scheduler is down...\n");
		outb(0x20, 0x20);
		while(1);
		//asm("hlt");
	}

	if(current->state == PROCSTATE_IDLE)// Sinon on signale que le processus est désormais actif
	{
		current->state = PROCSTATE_RUNNING;
	}
	
	//syscall_update_esp(current->sys_stack);
	
	
	// Mise en place de l'interruption sur le quantum de temps
	event_id = add_event(schedule,NULL,quantum);	
	i8254_init(1000/*TIMER_FREQ*/);

	// On réaffecte à la main stdin, stdout et stderr. TEMPORAIRE ! Il faudrait que stdin, stdout et stderr soient tjs à la même adresse pour chaque processus...
	stdin = current->stdin;
	stdout = current->stdout;
	stderr = current->stderr;

	// Changer le contexte:
	if(current->regs.cs == 0x8)
		process_switch(KERNEL_PROCESS, current);
	else
		process_switch(USER_PROCESS, current);
		

	return NULL;
}

void init_scheduler(int Q)
{
	quantum = Q;
}

void stop_scheduler()
{
	del_event(event_id);
}

void start_scheduler()
{
	event_id = add_event(schedule,NULL,quantum);
}

void* sys_exec(paddr_t prog, char* name, uint32_t unused __attribute__ ((unused)))
{
	char ** argv = (char **) kmalloc(sizeof(char*));
	argv[0] = strdup(name);
	create_process(name, prog, 1, argv, 0x1000, 3);
	return NULL;
}

void* sleep_callback( void* data )
{
	process_t* proc = (process_t*) data;
	proc->state = PROCSTATE_RUNNING;
	//kprintf("A");
	return NULL;
}

void* sys_sleep( uint32_t delay,uint32_t unused2 __attribute__ ((unused)), uint32_t unused3 __attribute__ ((unused)))
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
	
		
	return NULL;
}

void exec(paddr_t prog, char* name)
{
	syscall(SYS_EXEC, (uint32_t)prog, (uint32_t)name, (uint32_t)NULL);
}
