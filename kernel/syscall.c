
//Pour plus d'infos sur sysenter: Doc Intel 2B 4-483

#include <msr.h>
#include <stdio.h>

uint32_t syscall_stack[1024];

void SysCallHandler()
{	
	printf("\nsyscall!\n");
	//while(1);
	asm ("sysexit\n\t");
}

void syscall_update_esp(paddr_t esp)
{
	write_msr(IA32_SYSENTER_ESP, esp , 0);
}

void init_syscall()
{
	write_msr(IA32_SYSENTER_CS,	1,	0);		// Selecteur du segment code kernel
	//write_msr(IA32_SYSENTER_ESP, syscall_stack+1023 , 0);		// Pointeur de pile kernel
	write_msr(IA32_SYSENTER_EIP, SysCallHandler , 0); 	// Pointeur d'instruction du handler
}
