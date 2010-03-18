
//Pour plus d'infos sur sysenter: Doc Intel 2B 4-483

#include <msr.h>
#include <stdio.h>

uint32_t syscall_stack[1024];

void SysCallHandler()
{	
	printf("syscall!\n");
	asm ("sysexit\n\t");
}

void init_syscall()
{
	write_msr(IA32_SYSENTER_CS,	1,	10);		// Selecteur du segment code kernel
	write_msr(IA32_SYSENTER_ESP, syscall_stack+1023 , 10);		// Pointeur de pile kernel
	write_msr(IA32_SYSENTER_EIP, SysCallHandler , 10); 	// Pointeur d'instruction du handler
}
