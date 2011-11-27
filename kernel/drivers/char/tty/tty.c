/**
 * @file tty.c
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

#include <tty.h>
#include <kmalloc.h>
#include <fs/devfs.h>
#include <string.h>
#include <ksem.h>
#include <kprocess.h>
#include <scheduler.h>
#include <ksignal.h>

#include <fcntl.h>

void tty_init() {

}

void tty_insert_flip_char(tty_struct_t *tty, unsigned char c) {
	if (tty == NULL) return;

	if (c == '\b') {
		if (tty->p_end != tty->p_begin) {
			tty->p_end--;
//		if (tty->echo) {
				if (tty->driver->ops->put_char != NULL) {
					tty->driver->ops->put_char(tty, c);
				} else {
					unsigned char ch[2];
					ch[0] = c;
					ch[1] = '\0';
					tty->driver->ops->write(tty, NULL, ch, 1);
				}
//		}
		}
	} else {
		if ((tty->p_end + 1) % MAX_INPUT != tty->p_begin) {
			tty->buffer[tty->p_end] = c;
			tty->p_end = (tty->p_end + 1) % MAX_INPUT;

//		if (tty->echo) {
				if (tty->driver->ops->put_char != NULL) {
					tty->driver->ops->put_char(tty, c);
				} else {
					unsigned char ch[2];
					ch[0] = c;
					ch[1] = '\0';
					tty->driver->ops->write(tty, NULL, ch, 1);
				}
//		}
		}
	}
	ksemV(tty->sem);
}

tty_driver_t *alloc_tty_driver(int lines) {
	tty_driver_t *driver = (tty_driver_t *)kmalloc(sizeof(tty_driver_t));
	driver->num = lines;
	return driver;
}

void put_tty_driver(tty_driver_t *driver) {
	kfree(driver);
}

static chardev_interfaces* tty_get_driver_interface(tty_struct_t *tty) {
	chardev_interfaces *di = kmalloc(sizeof(chardev_interfaces));
	di->read = tty_read;
	di->write = tty_write;
	di->open = tty_open;
	di->close = tty_close;
	di->ioctl = tty_ioctl;
	di->custom_data = (void*)tty;
	return di;
}

int tty_register_driver(tty_driver_t *driver) {
	int n;
	driver->ttys = kmalloc(sizeof(tty_struct_t*) * driver->num);
	for (n = 0; n < driver->num; n++) {
		int len = strlen(driver->devfs_name);
		char *name = kmalloc(strlen(driver->devfs_name) + 5);
		int i;
		for (i = 0; i < len; i++) {
			name[i] = driver->devfs_name[i];
		}
		int first = 1;
		int j;
		int num = n;
		for (j = 10000; j >= 1; j /= 10) {
			int d = num / j;
			if (d > 0 || !first || j == 1) {
				name[i++] = '0' + d;
				num -= d * j;
				first = 0;
			}
		}
		name[i] = '\0';
		driver->ttys[n] = kmalloc(sizeof(tty_struct_t));
		driver->ttys[n]->driver = driver;
		driver->ttys[n]->termios = driver->init_termios;
		driver->ttys[n]->p_begin = 0;
		driver->ttys[n]->p_end = 0;
		driver->ttys[n]->sem = ksemcreate_without_key(NULL);
		driver->ttys[n]->index = n;
		register_chardev(name, tty_get_driver_interface(driver->ttys[n]));
	}
	return 0;
}

size_t tty_write(open_file_descriptor *ofd, const void *buf, size_t count) {
	/*
	 * Copié de l'ancien tty.
	 */

  if((ofd->flags & O_ACCMODE) == O_RDONLY) {
    //errno = EBADF;
    return -1;
  }

	chardev_interfaces *di = ofd->extra_data;
	tty_struct_t *t = (tty_struct_t *) di->custom_data;
	process_t *current_process = get_current_process();

	if (current_process->pid != t->fg_process) {
		sys_kill(current_process->pid, SIGTTOU, NULL);
	}

	return t->driver->ops->write(t, ofd, buf, count);
}

size_t tty_read(open_file_descriptor *ofd, void *buf, size_t count) {

	/*
	 * Copié de l'ancien driver !
	 */
	char c;
	unsigned int j = 0;
	chardev_interfaces *di = ofd->extra_data;
	tty_struct_t *t = (tty_struct_t *) di->custom_data;

	process_t *current_process = get_current_process();

	if (current_process->pid != t->fg_process) {
		sys_kill(current_process->pid, SIGTTIN, NULL);
	}

  if((ofd->flags & O_ACCMODE) == O_WRONLY) {
    //errno = EBADF;
    return -1;
  }

  if(ofd->flags & O_DIRECT) {
    //errno = EINVAL;
    return -1;
  }

	do {
		if (t->p_begin == t->p_end && !(ofd->flags & O_NONBLOCK))
			ksemP(t->sem);

		c = t->buffer[(t->p_end + MAX_INPUT - 1) % MAX_INPUT];

		if (/*!t->canon ||*/ c == '\n' || c == '\r') {
			while (j < count && t->p_begin < t->p_end) {
				((char*) buf)[j] = t->buffer[t->p_begin];
				t->p_begin = (t->p_begin + 1) % MAX_INPUT;
				j++;
			}
			break;
		} else {
			if (ofd->flags & O_NONBLOCK) {
				return -1;
			}
			ksemP(t->sem);
		}
	} while (1);

	return j;
}

int tty_ioctl (open_file_descriptor *ofd __attribute__ ((unused)), unsigned int request __attribute__ ((unused)), void *data __attribute__ ((unused))) {
	return 0;
}

int tty_close (open_file_descriptor *ofd __attribute__ ((unused))) {
	return 0;
}

int tty_open (open_file_descriptor *ofd __attribute__ ((unused))) {
	return 0;
}

