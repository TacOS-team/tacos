#include <types.h>
#include <interrupts.h>

static bool irq_received = FALSE;

interrupt_handler_t floppy_handler(int interrupt_id)
{
	irq_received = TRUE;
}

void floppy_wait_irq()
{
	while(!irq_received){}
	
	irq_received = FALSE;
}

void floppy_init_interrupt()
{
	interrupt_set_routine(IRQ_FLOPPY, floppy_handler);
}

void floppy_reset_irq()
{
	irq_received = FALSE;
}


