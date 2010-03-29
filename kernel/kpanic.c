#include <stdio.h>
#include <exception.h>
#include <kpanic.h>

void printStackTrace(uint32_t depth)
{
	// Joli petit hack:
	// on récupère ebp à partir de l'adresse de l'argument:
	uint32_t* ebp = &depth - 2;
	uint32_t i;
	printf("Stack Trace:\n");
	
	for(i=0; i<depth; ++i)
	{
		uint32_t eip = ebp[1];
		
		if(eip == 0x0010001f) // Si on arrive au multiboot_entry
		{
			break;
		}
		ebp = ebp[0];
		printf("->0x%x\n",eip);
	}
}

void page_fault_report(int error_code)
{
	uint32_t address;
			
	/* On récupère le registre cr2 qui contient l'addresse virtuelle à l'origine de l'exception */
	asm("mov %%cr2, %%eax":"=a"(address));
	printf("Virtual address: 0x%x\n",address);

	if(error_code & 0x01) // Bit P
		printf("Page-protection violation.\n");
	else
		printf("Non-present page.\n");

	printf("Read/Write: ");
	if(error_code & 0x02) // Bit W
		printf("write\n");
	else
		printf("reading\n");

	printf("Privilege=");
	if(error_code & 0x04) // Bit U
		printf("3\n");
	else
		printf("0\n");
		
	printf("Reserved write: ");
	if(error_code & 0x08) // Bit R
		printf("yes\n");
	else
		printf("no\n");
		
	printf("Instruction fetch: ");
	if(error_code & 0x10) // Bit I
		printf("yes\n");
	else
		printf("no\n");
}
	
void kpanic_handler(int error_id, int error_code)
{
  //cls();
  set_attribute(WHITE, BLACK);
  cls();
  printf("                              /!\\ KERNEL PANIC /!\\\n");
  printf("--------------------------------------------------------------------------------\n");
  printf("Exception handled : ");
  switch(error_id)
  {
		case EXCEPTION_SEGMENT_NOT_PRESENT:
			printf("Segment not present (error code : %d).\n", error_code);
			break;
		case EXCEPTION_DIVIDE_ERROR:
			printf("Division by zero.\n");
			break;
		case EXCEPTION_INVALID_OPCODE:
			printf("Invalid OpCode.\n");
			break;
		case EXCEPTION_INVALID_TSS:
			printf("Invalid TSS (error code : %d).\n", error_code);
			break;
		case EXCEPTION_PAGE_FAULT:
			printf("Page fault (error code : %d).\n", error_code);
			page_fault_report(error_code);
			break;
		case EXCEPTION_DOUBLE_FAULT:
			printf("Double fault (error code : %d).\n", error_code);
			break;
		default:
			printf("Unknown exception.\n");
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
}

