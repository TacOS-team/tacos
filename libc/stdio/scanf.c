/**
 * @file scanf.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2015 TacOS developers.
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

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <libio.h>
#include <string.h>

#define        conv_error()   return -1
#define        input_error()  return (done == (0) ? EOF : done)

int vsscanf(const char *s, const char *format, va_list ap) {
	FILE stream;
	stream._fileno = -1;
	stream._IO_buf_base = (char*) s;
	stream._IO_buf_end =(char*) (s + strlen(s));
	*stream._IO_buf_end++ = EOF;
	stream._IO_read_base =(char*) s;
	stream._IO_read_ptr = stream._IO_buf_end;
	stream._IO_read_end = (char*) s;
	stream._flags = _IO_MAGIC + _IO_UNBUFFERED;
	return vfscanf(&stream, format, ap);
}

int sscanf(const char *s, const char *format, ...) {
	int result;
	va_list ap;

	va_start(ap, format);
	result = vsscanf(s, format, ap);
	va_end(ap);
	return result;
}

/* Code fortement inspiré de la GNU C Library. */
int vfscanf(FILE *stream, const char *format, va_list ap) {
  const char *f = format;
  char fc;                /* Current character of the format.  */
  int done = 0;        /* Assignments done.  */
  char c;                /* Last char read.  */

  /* Base for integral numbers.  */
  int base;
  /* Signedness for integral numbers.  */
  int number_signed = 0;
  /* Integral holding variables.  */
  long int num;
  unsigned long int unum;
  /* Character-buffer pointer.  */
  register char *str;
  /* Workspace.  */
  char work[200];
  char *w;

  c = fgetc(stream);

  /* Run through the format string.  */
  while (*f != '\0') {
		fc = *f++;
		if (fc != '%') {
			/* Characters other than format specs must just match.  */
			if (c == EOF) {
				input_error();
			}
			if (isspace(fc)) {
				/* Whitespace characters match any amount of whitespace.  */
				while (isspace (c)) {
					c = fgetc(stream);
				}
				continue;
			} else if (c == fc) {
				c = fgetc(stream);
			} else {
				conv_error();
			}
			continue;
		}

		/* End of the format string?  */
		if (*f == '\0') {
			conv_error();
		}

		/* Find the conversion specifier.  */
		w = work;
		fc = *f++;
		switch (fc) {
		  case '%':		  /* Must match a literal '%'.  */
				if (c != fc)
					conv_error();
				break;

			case 'c':        /* Match characters.  */
				str = va_arg (ap, char *);
				if (str == NULL)
					conv_error ();

				if (c == EOF)
					input_error();

				break;

		  case 's':					 /* Read a string.  */
				str = va_arg (ap, char *);
				if (str == NULL)
				 conv_error ();

				// XXX: ignore all whitespace characters
				while (isspace(c) || c == '\n') {
					c = fgetc(stream);
				}

				if (c == EOF)
				 input_error ();

				do {
					if (isspace (c) || c == '\n') {
						// End of string: put back the whitespace character in the buffer
						fseek(stream, -1, SEEK_CUR);
						break;
					}
				 	*str++ = c; 
				} while ((c = fgetc(stream)) != EOF);

				*str = '\0';
				++done;
				break;

		  case 'x':		  /* Hexadecimal integer.  */
		  case 'X':		  /* Ditto.  */ 
				base = 16;
				number_signed = 0;
				goto number;

		  case 'o':		  /* Octal integer.  */
				base = 8;
				number_signed = 0;
				goto number;

		  case 'u':		  /* Unsigned decimal integer.  */
				base = 10;
				number_signed = 0;
				goto number;

		  case 'd':		  /* Signed decimal integer.  */
				base = 10;
				number_signed = 1;
				goto number;

		  case 'i':		  /* Generic number.  */
				base = 0;
				number_signed = 1;

		  number:
				// XXX: ignore all whitespace characters
				while (isspace(c) || c == '\n') {
					c = fgetc(stream);
				}

				if (c == EOF)
				 input_error();

				/* Check for a sign.  */
				if (c == '-' || c == '+') {
					*w++ = c;
					c = fgetc(stream);
				}

				/* Look for a leading indication of base.  */
				if (c == '0') {
					*w++ = '0';

					c = fgetc(stream);

					if (tolower(c) == 'x') {
				 		if (base == 0) {
				 		  base = 16;
						}
				 		if (base == 16) {
							c = fgetc(stream);
				 		}
				 	} else if (base == 0) {
						base = 8;
					}
				}

				if (base == 0)
				 base = 10;

				/* Read the number into WORK.  */
				do {
					if (base == 16 ? !isxdigit(c) :
				 		(!isdigit(c) || c - '0' >= base))
				 	 break;
						*w++ = c;
				} while ((c = fgetc(stream)) != EOF);

				if (w == work ||
						(w - work == 1 && (work[0] == '+' || work[0] == '-')))
				 /* There was on number.  */
				 conv_error();

				/* Convert the number.  */
				*w = '\0';
				if (number_signed)
					num = strtol (work, &w, base);
				else
					unum = strtoul (work, &w, base);
				if (w == work)
					conv_error ();

				if (!number_signed) {
					*va_arg(ap, unsigned int *) = (unsigned int) unum;
				} else {
					*va_arg(ap, int *) = (int) num;
				}
				++done;
				break;

		  case 'p':		  /* Generic pointer.  */
				base = 16;
				goto number;
		}
	}

  return done;

}

int fscanf(FILE *stream, const char *format, ...) {
	int result;
	va_list ap;

	va_start(ap, format);
	result = vfscanf(stream, format, ap);
	va_end(ap);

	return result;
}

int scanf(const char *format, ...) {
	int result;
	va_list ap;

	va_start(ap, format);
	result = vfscanf(stdin, format, ap);
	va_end(ap);

	return result;
}
