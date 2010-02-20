#ifndef _i8259_H_
#define _i8259_H_

void i8259_setup(void);
void i8259_enable_irq_line(uint8_t n);
void i8259_disable_irq_line(uint8_t n);

#endif
