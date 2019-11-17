/**
 * @file kstdio.c
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

#define __KTW_COLUMNS 80
#define __KTW_LINES 25

#include <kfcntl.h>
#include <tty.h>
#include <vfs.h>
#include <fs/devfs.h>
#include <klibc/stdlib.h>
#include <klibc/stdarg.h>


void kprintf(const char *format, ...) {
	va_list arg;

	va_start(arg, format);

	static tty_struct_t *tty = NULL;
	if (tty == NULL) {
		open_file_descriptor* ofd = vfs_open("/dev/ttyS0", 0); //XXX:flags
		if (ofd) {
			tty = ((chardev_interfaces*)ofd->i_fs_specific)->custom_data;
		} else {
			va_end(arg);
			return;
		}
	}


	int c;
	char buf[20];


	while ((c = *format++) != 0) {
		if (c != '%') {
			//ofd->f_ops->write(ofd, &c, 1);
			tty->driver->ops->put_char(tty, c);
		} else {
			char *p;
			int long_modifier = 0;

			c = *format++;

			while (c == 'l') {
				long_modifier++;
				c = *format++;
			}

			switch (c) {
			case 'c':
				tty->driver->ops->put_char(tty, va_arg(arg, int));
				break;
			case 'd':
			case 'u':
				if (long_modifier == 2) {
					itoa(buf, c, va_arg(arg, long long));
				} else if (long_modifier == 1) {
					itoa(buf, c, va_arg(arg, long));
				} else {
					itoa(buf, c, va_arg(arg, int));
				}
				p = buf;
				goto string;
				break;
			case 'x':
				itox(buf, va_arg(arg, int));
				p = buf;
				goto string;
				break;
			case 's':
				p = va_arg(arg, char*);
				if (!p)
					p = "(null)";

				string:
					//ofd->f_ops->write(ofd, (unsigned char*) p, 1024);
					tty->driver->ops->write(tty, NULL, (unsigned char*) p, 1024);
					break;

			default:
				//ofd->f_ops->write(ofd, (int *) arg++, 1);
				tty->driver->ops->put_char(tty, c);
				break;
			}
		}
	}

	va_end(arg);
}



void sprintf(char *str, const char *format, ...) {
	va_list arg;

	va_start(arg, format);

	int c;
	char buf[20];

	while ((c = *format++) != 0) {
		if (c != '%') {
			*str = c;
			str++;
		} else {
			char *p = NULL;
			int long_modifier = 0;
			c = *format++;

			while (c == 'l') {
				long_modifier++;
				c = *format++;
			}

			switch (c) {
			case 'c':
				*str = va_arg(arg, int);
				str++;
				break;
			case 'd':
			case 'u':
			case 'x':
				if (long_modifier == 2) {
					itoa(buf, c, va_arg(arg, long long));
				} else if (long_modifier == 1) {
					itoa(buf, c, va_arg(arg, long));
				} else {
					itoa(buf, c, va_arg(arg, int));
				}
				p = buf;
				/* Falls through. */
			case 's':
				if (p == NULL) {
					p = va_arg(arg, char*);
					if (!p)
						p = "(null)";
				}
				while (*p) {
					*str = *p;
					str++;
					p++;
				}
				break;
			default:
				*str = c;
				break;
			}
		}
	}
	*str = '\0';
	va_end(arg);
}
