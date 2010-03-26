#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#define IRQ_TIMER           0
#define IRQ_KEYBOARD        1
#define IRQ_SLAVE_PIC       2
#define IRQ_COM2            3
#define IRQ_COM1            4
#define IRQ_RESERVED_1      5 // sound card
#define IRQ_FLOPPY          6
#define IRQ_LPT1            7
#define IRQ_8_REAL_TIME_CLK 8
#define IRQ_REDIRECT_IRQ2   9
#define IRQ_RESERVED_2      10
#define IRQ_RESERVED_3      11
#define IRQ_PS2_MOUSE       12
#define IRQ_COPROCESSOR     13
#define IRQ_HARDDISK        14
#define IRQ_RESERVED_4      15
#define IRQ_SYSCALL			16

typedef void (*interrupt_handler_t)(int interrupt_id);

int interrupt_set_routine(uint8_t interrupt_id, interrupt_handler_t routine, uint8_t privilege);
int interrupt_disable(uint8_t interrupt_id);

#endif
