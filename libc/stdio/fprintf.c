/**
 * @file fprintf.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010 - TacOS developers.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * @section DESCRIPTION
 *
 * Description de ce que fait le fichier
 */

/**
 * @file fprintf.c
 */

#include <stdio.h>
#include <string.h>

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
	int n = 0;

	while ((c = *format++) != 0) {
		if (c != '%') {
			if (fputc(c, stream) == EOF) {
				return -1;
			}
			n++;
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
					if (fputc(va_arg(ap, int), stream) == EOF) {
						return -1;
					}
					n++;
					break;
				case 's':
					p = va_arg(ap, char *);
					if (p == NULL)
						p = "(null)";
				string:
					if (size >= 0) {
						p[size] = '\0';
						n += size;
					} else {
						n += strlen(p);
					}
					if (fputs(p, stream) == EOF) {
						return -1;
					}
					break;
				default:
					if (fputc('%', stream) == EOF || fputc(c, stream) == EOF) {
						return -1;
					}
					n += 2;
					break;
			}
		}
	}
	return n;
}
