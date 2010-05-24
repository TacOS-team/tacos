
//Pour plus d'infos sur sysenter: Doc Intel 2B 4-483

#include <msr.h>
#include <stdio.h>
#include <string.h>
#include <interrupts.h>
#include "ksyscall.h"
#include <syscall.h>

syscall_handler_t syscall_handler_table[MAX_SYSCALL_NB];


void syscall_entry(int interrupt_id __attribute__ ((unused)))
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
		kprintf("ERROR: Unknown syscall handler.\n");
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
	//make_trapgate_from_int(IRQ_SYSCALL);
	// Mise à zero de la table des handler
	memset(syscall_handler_table, 0, MAX_SYSCALL_NB*sizeof(syscall_handler_t));
}
