#ifndef _STDIO_H_
#define _STDIO_H_

void disableCursor();
void cls (void);
void itoa (char *buf, int base, int d);
void newline();
void putchar (int c);
void printf (const char *format, ...);
void enableFinnouMode(int enable);

#endif
