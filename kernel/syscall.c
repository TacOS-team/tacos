
//Pour plus d'infos sur sysenter: Doc Intel 2B 4-483

#include <msr.h>
#include <stdio.h>
#include <interrupts.h>
#include "syscall.h"

syscall_handler_t syscall_handler_table[MAX_SYSCALL_NB];


void syscall_entry(int interrupt_id)
{	
	uint32_t function, param1, param2, param3;
	syscall_handler_t handler;
	
	// On récupère les parametres
	asm("":"=a"(function),"=b"(param1),"=c"(param2),"=d"(param3));

	// On execute le handler correspondant
	if(function < MAX_SYSCALL_NB && syscall_handler_table[function] != NULL)
	{
		handler = syscall_handler_table[function];
		handler(param1, param2, param3);
	}
	else
		printf("ERROR: Unknown syscall handler.\n");
}

int syscall_set_handler(uint32_t syscall_id, syscall_handler_t handler)
{
	int ret = -1;
	if(syscall_id < MAX_SYSCALL_NB && syscall_handler_table[syscall_id] == NULL)
	{
		syscall_handler_table[syscall_id] = handler;
		ret = 0;
	}
	return ret;
}

void init_syscall()
{
	interrupt_set_routine(IRQ_SYSCALL, syscall_entry, 3);
	
	// Mise à zero de la table des handler
	memset(syscall_handler_table, NULL, MAX_SYSCALL_NB*sizeof(syscall_handler_t));
}

void syscall(uint32_t func, uint32_t param1, uint32_t param2, uint32_t param3)
{
	// On met la fonction et les parametres dans les bon registres
	asm(""::"a"(func),"b"(param1),"c"(param2),"d"(param3));
	
	// et on lance l'interruption
	asm("int $0x30");
}
	
