#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

/* TODO : Define des différentes irq possibles ? j'hésite parce que c'est pas fixé... */

#ifndef ASM_SOURCE

typedef void (*interrupt_handler_t)(int interrupt_id);

int interrupt_set_routine(uint8_t interrupt_id, interrupt_handler_t routine);
int interrupt_disable(uint8_t interrupt_id);

#endif

#endif
