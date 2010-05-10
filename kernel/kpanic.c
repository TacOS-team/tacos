#include <stdio.h>
#include <exception.h>
#include <kpanic.h>

void printStackTrace(uint32_t depth)
{
	// Joli petit hack:
	// on récupère ebp à partir de l'adresse de l'argument:
	uint32_t* ebp = &depth - 2;
	uint32_t i;
	kprintf("Stack Trace:\n");
	
	for(i=0; i<depth; ++i)
	{
		uint32_t eip = ebp[1];
		
		if(eip == 0x0010001f) // Si on arrive au multiboot_entry
		{
			break;
		}
		ebp = (uint32_t*) ebp[0];
		kprintf("->0x%x\n",eip);
	}
}

void page_fault_report(int error_code)
{
	uint32_t address;
			
	/* On récupère le registre cr2 qui contient l'addresse virtuelle à l'origine de l'exception */
	asm("mov %%cr2, %%eax":"=a"(address));
	kprintf("Virtual address: 0x%x\n",address);

	if(error_code & 0x01) // Bit P
		kprintf("Page-protection violation.\n");
	else
		kprintf("Non-present page.\n");

	kprintf("Read/Write: ");
	if(error_code & 0x02) // Bit W
		kprintf("write\n");
	else
		kprintf("reading\n");

	kprintf("Privilege=");
	if(error_code & 0x04) // Bit U
		kprintf("3\n");
	else
		kprintf("0\n");
		
	kprintf("Reserved write: ");
	if(error_code & 0x08) // Bit R
		kprintf("yes\n");
	else
		kprintf("no\n");
		
	kprintf("Instruction fetch: ");
	if(error_code & 0x10) // Bit I
		kprintf("yes\n");
	else
		kprintf("no\n");
}
	
void kpanic_handler(int error_id, int error_code)
{
	// background white
	kprintf("\033[47m");
	// Foreground black
	kprintf("\033[30m");
	// cls :
	kprintf("\033[2J");

	fflush(stdout);
	kprintf("                              /!\\ KERNEL PANIC /!\\\n");
	kprintf("--------------------------------------------------------------------------------\n");
	kprintf("Exception handled : ");
	switch(error_id)
	{
		case EXCEPTION_SEGMENT_NOT_PRESENT:
			kprintf("Segment not present (error code : 0x%x).\n", error_code);
			break;
		case EXCEPTION_DIVIDE_ERROR:
			kprintf("Division by zero.\n");
			break;
		case EXCEPTION_INVALID_OPCODE:
			kprintf("Invalid OpCode.\n");
			break;
		case EXCEPTION_INVALID_TSS:
			kprintf("Invalid TSS (error code : %d).\n", error_code);
			break;
		case EXCEPTION_PAGE_FAULT:
			kprintf("Page fault (error code : %d).\n", error_code);
			page_fault_report(error_code);
			break;
		case EXCEPTION_DOUBLE_FAULT:
			kprintf("Double fault (error code : %d).\n", error_code);
			break;
		case EXCEPTION_GENERAL_PROTECTION :
			kprintf("General Proctection fault (error code : %d).\n", error_code);
			break;
		default:
			kprintf("Unknown exception.\n");
	}
	printStackTrace(10);
	asm("cli");
	asm("hlt");
}

void kpanic_init()
{
  exception_set_routine(EXCEPTION_SEGMENT_NOT_PRESENT , kpanic_handler);
  exception_set_routine(EXCEPTION_DIVIDE_ERROR, kpanic_handler);
  exception_set_routine(EXCEPTION_INVALID_OPCODE, kpanic_handler);
  exception_set_routine(EXCEPTION_INVALID_TSS, kpanic_handler);
  exception_set_routine(EXCEPTION_PAGE_FAULT, kpanic_handler);
  exception_set_routine(EXCEPTION_DOUBLE_FAULT, kpanic_handler);
  exception_set_routine(EXCEPTION_GENERAL_PROTECTION, kpanic_handler);
}

