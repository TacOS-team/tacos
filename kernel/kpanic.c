#include <stdio.h>
#include <exception.h>
#include <kpanic.h>

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

