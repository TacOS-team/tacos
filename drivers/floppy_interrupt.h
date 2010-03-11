#ifndef _FLOPPY_INTERRUPT_H_
#define _FLOPPY_INTERRUPT_H_

void floppy_init_interrupt();
void floppy_reset_irq();
void floppy_wait_irq();
void floppy_sense_interrupt(int base, int* st0, int* cy1);

#endif
