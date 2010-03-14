#include <stdio.h>
#include <exception.h>
#include <kpanic.h>

void kpanic_handler(int error_id)
{
  //cls();
  set_attribute(WHITE, BLACK);
  printf("/!\\ KERNEL PANIC /!\\\n");
  printf("Exception handled : %d\n\n", error_id);

  asm("cli");
  asm("hlt");
}

void kpanic_init()
{
  exception_set_routine(EXCEPTION_DIVIDE_ERROR, kpanic_handler);
}

