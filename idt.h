#ifndef _IDT_H_
#define _IDT_H_

#include <types.h>

void idt_setup();
int idt_set_handler(uint8_t index, paddr_t handler_address, uint8_t priority);
void idt_disable_handler(uint8_t index);

#endif
