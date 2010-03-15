#include <stdio.h>
#include <exception.h>
#include <kpanic.h>

void printStackTrace(uint32_t depth)
{
	// Joli petit hack:
	// on récupère ebp à partir de l'adresse de l'argument:
	uint32_t* ebp = &depth - 2;
	uint32_t i;
	printf("Stack Tace:\n");
	
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
	
void kpanic_handler(int error_id)
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
			printf("Segment not present.\n");
			break;
		case EXCEPTION_DIVIDE_ERROR:
			printf("Division by zero.\n");
			break;
		case EXCEPTION_INVALID_OPCODE:
			printf("Invalid OpCode.\n");
			break;
		case EXCEPTION_INVALID_TSS:
			printf("Invalid TSS.\n");
			break;
		case EXCEPTION_PAGE_FAULT:
			printf("Page fault.\n");
			break;
		case EXCEPTION_DOUBLE_FAULT:
			printf("Double fault.\n");
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

