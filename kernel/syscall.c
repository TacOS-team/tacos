
//Pour plus d'infos sur sysenter: Doc Intel 2B 4-483

#include <msr.h>
#include <stdio.h>
#include <interrupts.h>
#include <types.h>


#define GET_SYSCALL_IRQ(x) # x

uint32_t syscall_stack[1024];

void SysCallHandler(int interrupt_id)
{	
	uint32_t function, param1, param2, param3;
	// On récupère les parametres
	asm("":"=a"(function),"=b"(param1),"=c"(param2),"=d"(param3));
	printf("\nsyscall n=0x%x(%d, %d, %d)\n", function, param1, param2, param3);

	// Ici on devrait executer la fonctions correspondante
}


void init_syscall()
{
	interrupt_set_routine(IRQ_SYSCALL,SysCallHandler, 3);
}

void syscall(uint32_t func, uint32_t param1, uint32_t param2, uint32_t param3)
{
	// On met la fonction et les parametres dans les bon registres
	asm(""::"a"(func),"b"(param1),"c"(param2),"d"(param3));
	
	// et on lance l'interruption
	asm("int $0x30");
}
	
