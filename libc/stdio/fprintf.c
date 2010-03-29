#include <stdio.h>

int fprintf(FILE *stream, const char *format, ...) {
    va_list arg;
    int result;
 
    va_start(arg, format);
    result = vfprintf(stream, format, arg);
    va_end(arg);
 
    return result;
}

int vfprintf(FILE *stream, const char *format, va_list ap) {
	int c;
	char buf[20];

	while ((c = *format++) != 0) {
		if (c != '%') {
			fputc(c, stream);
		} else {
			char *p;

			c = *format++;
			switch (c) {
				case 'd':
				case 'u':
				case 'x':
					itoa(buf, c, va_arg(ap, int));
					p = buf;
					goto string;
					break;
				case 's':
					p = va_arg(ap, char *);
					if (p == NULL)
						p = "(null)";
				string:
					fputs(p, stream);
					break;
				default:
					fputc('%', stream);
					fputc(c, stream);
					break;
			}
		}
	}
}
