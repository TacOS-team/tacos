/**
 * @file kstdio.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012, 2013 - TacOS developers.
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

void kprintf(const char *format, ...) {
	static tty_struct_t *tty = NULL;
	if (tty == NULL) {
		open_file_descriptor* ofd = vfs_open("/dev/ttyS0", 0); //XXX:flags
		if (ofd)
			tty = ((chardev_interfaces*)ofd->i_fs_specific)->custom_data;
		else
			return;
	}

	char **arg = (char **) &format;
	int c;
	char buf[20];

	arg++;

	while ((c = *format++) != 0) {
		if (c != '%') {
			//ofd->f_ops->write(ofd, &c, 1);
			tty->driver->ops->put_char(tty, c);
		} else {
			char *p;

			c = *format++;
			switch (c) {
			case 'd':
			case 'u':
				itoa(buf, c, *((int *) arg++));
				p = buf;
				goto string;
				break;
			case 'x':
				itox(buf,*((int *) arg++));
				p = buf;
				goto string;
				break;
			case 's':
				p = *arg++;
				if (!p)
					p = "(null)";

				string:
					//ofd->f_ops->write(ofd, (unsigned char*) p, 1024);
					tty->driver->ops->write(tty, NULL, (unsigned char*) p, 1024);
					break;

			default:
				//ofd->f_ops->write(ofd, (int *) arg++, 1);
				tty->driver->ops->put_char(tty, *((int *) arg++));
				break;
			}
		}
	}
}
