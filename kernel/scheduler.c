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

process_t* idle_process;
static uint32_t quantum;						// Quantum de temps alloué aux process

static void* switch_process(void* data __attribute__ ((unused)))
{
	uint32_t* stack_ptr;
	uint32_t esp0, eflags;
   uint16_t ss, cs;
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
			current->regs.esp = stack_ptr[10] + 20;
		}
		else
		{
			current->regs.ss = stack_ptr[19];
			current->regs.esp = stack_ptr[18];
		}
		
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
	    
	    add_event(switch_process,NULL,quantum);	
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
	get_default_tss()->esp0 = current->kstack.esp0;
	
	// Mise en place de l'interruption sur le quantum de temps
	add_event(switch_process,NULL,quantum);	
	i8254_init(1000/*TIMER_FREQ*/);

	// On réaffecte à la main stdin, stdout et stderr. TEMPORAIRE ! Il faudrait que stdin, stdout et stderr soient tjs à la même adresse pour chaque processus...
	stdin = current->stdin;
	stdout = current->stdout;
	stderr = current->stderr;

	// Changer le contexte:
	ss = current->regs.ss;
	cs = current->regs.cs;
	esp0 = current->regs.esp;
	eflags = (current->regs.eflags | 0x200) & 0xFFFFBFFF; // Flags permettant le changemement de contexte
		
	asm(
			"mov %0, %%esp\n\t"
			/* On push les registres necessaires au changement de contexte */
			"push %1\n\t"
			"push %2\n\t"
			"push %3\n\t"
			"push %4\n\t"
			"push %5\n\t"
			::        
			"m" (esp0), 		
			"m" (ss), 					
			"m" (current->regs.esp), 	
			"m" (eflags), 				
			"m" (cs), 					
			"m" (current->regs.eip) 	
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
	return NULL;
}

void init_scheduler(int Q)
{
	quantum = Q;
}

void start_scheduler()
{
	add_event(switch_process,NULL,quantum);
	BOCHS_BREAKPOINT;
}

void* sys_exec(paddr_t prog, char* name, uint32_t unused __attribute__ ((unused)))
{
	create_process(name, prog, 42, 0xba, 512, 3);
	return NULL;
}

void* sys_idle( uint32_t unused1 __attribute__ ((unused)),uint32_t unused2 __attribute__ ((unused)), uint32_t unused3 __attribute__ ((unused)))
{
	unsigned int i = 0;
	kprintf("Idling...");
	//for(i = 0; i<100000; i++);
	//while(1);
	asm("hlt\n\t");
	kprintf("Not idling anymore.\n");
	return NULL;
}

void exec(paddr_t prog, char* name)
{
	syscall(SYS_EXEC, (uint32_t)prog, (uint32_t)name, (uint32_t)NULL);
}

void idle()
{
	syscall(SYS_IDLE, NULL, NULL, NULL);
}
