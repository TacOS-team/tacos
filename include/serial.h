#ifndef _SERIAL_H
#define _SERIAL_H

/* Masques pour les flags */
#define ECHO_ENABLED 0x2


typedef enum {
	COM1, COM2, COM3, COM4
}serial_port;

int serial_init(serial_port port, char* protocol, unsigned int bauds, int flags);

int serial_puts(serial_port port, char* string);
int serial_putc(serial_port port, char c);
int serial_gets(serial_port port, char* buffer, unsigned int size);

#endif /* _SERIAL_H_ */
