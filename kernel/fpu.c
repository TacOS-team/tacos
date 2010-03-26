#include <stdio.h>
#include <fpu.h>
#include <interrupts.h>

static void FPU_handler(int id)
{
  printf("FPU Exception\n");
}

void init_fpu()
{
	interrupt_set_routine(IRQ_COPROCESSOR, FPU_handler, 0);
  // FINIT : Initialize FPU after checking for pending unmasked floating-point exceptions.
  asm("finit");
}
