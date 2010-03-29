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
			int size = -1;
			char *p;

			c = *format++;

			if(c == '.')
			{
			  char c2;
			  size = 0;
			 
			  for(c2 = *format++; c2>='0' && c2<='9' ;c2 = *format++)
						 size = size * 10 + (c2 - '0');
			 
			  c = *(format-1);
			}

			switch (c) {
				case 'd':
				case 'u':
				case 'x':
					itoa(buf, c, va_arg(ap, int));
					p = buf;
					goto string;
					break;
				case 'c':
					fputc(va_arg(ap, char), stream);
					break;
				case 's':
					p = va_arg(ap, char *);
					if (p == NULL)
						p = "(null)";
				string:
					if (size >= 0) {
						p[size] = '\0';
					}
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
