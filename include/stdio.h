#ifndef _STDIO_H_
#define _STDIO_H_

#include <fcntl.h>
#include <types.h>
#include <stdarg.h>

/* End Of File character. */
#ifndef EOF
# define EOF (-1)
#endif

#define _IOFBF 0 /* Fully buffered. */
#define _IOLBF 1 /* Line buffered. */
#define _IONBF 2 /* No buffering. */

size_t write_screen(open_file_descriptor *ofd, const void *buf, size_t count);

void kprintf(const char *format, ...);
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

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

char *fgets(char *s, int size, FILE *stream);
int fgetc(FILE *stream);
int getchar(void);

int fflush(FILE *stream);

FILE *fopen(const char *path, const char *mode);
FILE *fdopen(int fd, const char *mode);
FILE *freopen(const char *path, const char *mode, FILE *stream);

FILE *fmemopen(void *buf, size_t size, const char *mode);

void init_stdfiles(FILE ** _stdin, FILE ** _stdout, FILE ** _stderr);

void itoa (char *buf, int base, int d);


#endif
