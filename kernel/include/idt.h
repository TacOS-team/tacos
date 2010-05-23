#ifndef _IDT_H_
#define _IDT_H_

/**
 * @file idt.h
 * @brief Initialisation de la table des descripteurs d'interruption et 
 * association aux handlers.
 */

#define IDT_TRAPGATE		0x7
#define IDT_INTGATE		0x6
#define IDT_TASKGATE 	0x5

#include <types.h>

void idt_setup();
int idt_set_handler(uint8_t index, paddr_t handler_address, uint8_t priority);
void idt_set_handler_type(uint8_t index, uint8_t trap_type);
void idt_disable_handler(uint8_t index);

#endif
