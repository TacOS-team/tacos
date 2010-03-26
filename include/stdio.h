#ifndef _STDIO_H_
#define _STDIO_H_

#include <types.h>
#include <stdarg.h>

#define BLACK         0x0
#define BLUE          0x1
#define GREEN         0x2
#define CYAN          0x3
#define RED           0x4
#define MAGENTA       0x5
#define BROWN         0x6
#define LIGHT_GRAY    0x7
#define DARK_GRAY     0x8
#define LIGHT_BLUE    0x9
#define LIGHT_GREEN   0xA
#define LIGHT_CYAN    0xB
#define LIGHT_RED     0xC
#define LIGHT_MAGENTA 0xD
#define YELLOW        0xE
#define WHITE         0xF

/* End Of File character. */
#ifndef EOF
# define EOF (-1)
#endif

void disableCursor();
void cls (void);
void itoa (char *buf, int base, int d);
int atoi(const char* __nptr);
void newline();
void putchar (int c);
void printf (const char *format, ...);
void sprintf (char* string, const char *format, ...);
void enableFinnouMode(int enable);
void set_attribute(uint8_t background, uint8_t foreground);
void reset_attribute();
void switchDebugBuffer();
void switchStandardBuffer();

int vsscanf(const char *s, const char *format, va_list ap);
int sscanf(const char *s, const char *format, ...);

#endif
