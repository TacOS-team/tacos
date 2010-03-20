#include <types.h>
#include <stdlib.h>
#include <stdio.h> 

#include "gdt.h"
#include "process.h"

typedef int (*main_func_type) (uint32_t, uint8_t**);

void exec_task(main_func_type func, uint32_t argc, uint8_t** argv)
{
		asm(
		"cli\n\t"
		"push %3\n\t"
		"push %2\n\t"
		"push $0x23\n\t"		//SS
		"push 0x1FFF0\n\t"		//ESP
		"pushfl\n\t"
		"popl %%eax\n\t"
		"orl $0x200, %%eax\n\t"
		"and $0xffffbfff, %%eax\n\t"
		"push %%eax\n\t"	// Flags
		"push $0x1B\n\t"	//CS
		"push %0\n\t"		//EIP
		"movl $0x1FFF0, %1\n\t"	// Sauvegarde de la pile kernel dans la TSS
		"movw $0x20, %%ax\n\t" 
		"movw %%ax, %%ds\n\t"
		"iret"
		:"=m"(func), "=m"(get_default_tss()->esp): "m"(argc), "m"(argv)
		);
}


int init_process(paddr_t prog, uint32_t argc, uint8_t** argv,process_t* new_proc)
{
	/* VIEUX CODE DE DEBUG
	int (*p)(uint32_t, uint8_t**) = (main_func_type)prog;
	p(argc,argv);
	*/
	
	/*
	 * non implemente
	 * TODO :
	 * mettre en place une stack
	 * mettre en place un segment data
	 * initialiser la stack (empiler les arguments...)
	 * remplir le struct process
	 */
	
	//new_proc->state = PROCSTATE_IDLE;
	new_proc->state = PROCSTATE_WAITING;
	
	return 1;
}

int cpu_ctxt_init (paddr_t* pStack, paddr_t* pFunct) {
  
  paddr_t CurrentSatck;
  asm volatile (\
   "movl %%esp, %0;\
    movl %1, %%esp;\
    pushl %2;\
    pushl %%ebp;\
    pushl $0;\
    pushw  %%gs;\
    pushw  %%fs;\
    pushw  %%ds;\
    pushw  %%ss;\
    pushl  %%edi;\
    pushl  %%esi;\
    pushl  %%ebp;\
    pushl  $0;\
    pushl  $0;\
    pushl  $0;\
    pushl  $0;\
    movl %%esp, %1;\
    movl %0, %%esp;"\
    ::\
    "m"(CurrentSatck),\
    "m"(*pStack),\
    "m"(pFunct)\
  );

  return 0;

}

void cpu_ctxt_switch(paddr_t* pOldStack, paddr_t* pNewSatck) {

  // A ce moment la (Appel de la function cpu_ctxt_switch) 
  // l'eip ext automatique stocké dans la pile comme adresse de retour
  // dans la pile ou l'on est c.a.d dans la pile de l'ancien processus
  // On stocke alors les flags et touts les registres cpu dans cette meme pile
  asm volatile (\
   "pushf;\
    pushw  %gs;\
    pushw  %fs;\
    pushw  %ds;\
    pushw  %ss;\
    pushl  %edi;\
    pushl  %esi;\
    pushl  %ebp;\
    pushl  %ebx;\
    pushl  %edx;\
    pushl  %ecx;\
    pushl  %eax;"\
  );
  // On change l'esp pour qu'il pointe sur la pile du nouveau processus
  asm volatile (\
   "movl %%esp, %0;\
    movl %1, %%esp;"\
    ::\
    "m"(*pOldStack),\
    "m"(*pNewSatck)\
  );
  // On retablie le nouveau contexte précédement sauvegardé dans la pile
  asm volatile (\
   "popl  %eax;\
    popl  %ecx;\
    popl  %edx;\
    popl  %ebx;\
    popl  %ebp;\
    popl  %esi;\
    popl  %edi;\
    popw  %ss;\
    popw  %ds;\
    popw  %fs;\
    popw  %gs;\
    popfl;"\
  );
  // L'instruction ret remet donc l'eip précedment sauvegardé
  asm volatile (" ret;");

}


