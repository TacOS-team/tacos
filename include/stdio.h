#ifndef _STDIO_H_
#define _STDIO_H_

#include <libio.h>
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

#define _IOFBF 0 /* Fully buffered. */
#define _IOLBF 1 /* Line buffered. */
#define _IONBF 2 /* No buffering. */

size_t write(int fd, const void *buf, size_t count);

int printf(const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);

int vprintf(const char *format, va_list ap);
int vfprintf(FILE *stream, const char *format, va_list ap);
int vsprintf(char *str, const char *format, va_list ap);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);

int scanf(const char *format, ...);
int sscanf(const char *s, const char *format, ...);
int fscanf(FILE *stream, const char *format, ...);

int vsscanf(const char *s, const char *format, va_list ap);
int vfscanf(FILE *stream, const char *format, va_list ap);
int vscanf(const char *format, va_list ap);

int fputc(int c, FILE *stream);
int fputs(const char *s, FILE *stream);
int putc(int c, FILE *stream);
int putchar(int c);
int puts(const char *s);

char *fgets(char *s, int size, FILE *stream);
int fgetc(FILE *stream);
int getchar(void);

int fflush(FILE *stream);

FILE *fopen(const char *path, const char *mode);
FILE *fdopen(int fd, const char *mode);
FILE *freopen(const char *path, const char *mode, FILE *stream);

void init_stdfiles(FILE ** _stdin, FILE ** _stdout, FILE ** _stderr);

// Vieux code pour affichage en 80x25...
void disableCursor();
void cls (void);
void itoa (char *buf, int base, int d);
int atoi(const char* __nptr);
void newline();
void enableFinnouMode(int enable);
void set_attribute(uint8_t background, uint8_t foreground);
void set_attribute_position(uint8_t background, uint8_t foreground, int x, int y);
void reset_attribute();
void switchDebugBuffer();
void switchStandardBuffer();

#endif
