#ifndef _SERIAL_H
#define _SERIAL_H

#include <types.h>

typedef enum {
	COM1, COM2, COM3, COM4
}serial_port;

int serial_init(serial_port port, char* protocol, unsigned int bauds);
void debug_puts(serial_port port, char* string);

#endif /* _SERIAL_H_ */
