/**
 * @file tty.c
 *
 * @author TacOS developers 
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
 * Gestion des terminaux tty.
 */

#include <fs/devfs.h>
#include <kfcntl.h>
#include <klibc/string.h>
#include <klog.h>
#include <kmalloc.h>
#include <kprocess.h>
#include <ksem.h>
#include <ksignal.h>
#include <scheduler.h>
#include <tty.h>

struct termios tty_std_termios = {
	.c_iflag = ICRNL | IXON,
	.c_oflag = OPOST | ONLCR,
	.c_cflag = CS8 | CREAD | HUPCL,
	.c_lflag = ISIG | ICANON | ECHO | ECHOE | ECHOK |
	     ECHOCTL | ECHOKE,
	.c_cc = INIT_C_CC,
	.c_ispeed = 38400,
};

#ifndef EOF
  #define EOF (-1)
#endif

#define I_ECHO(tty) (tty->termios.c_lflag & ECHO)			/**< Echo activé ? */
#define I_CANON(tty) (tty->termios.c_lflag & ICANON)	/**< Mode canonique ? */

#define I_IGNCR(tty) (tty->termios.c_iflag & IGNCR)		/**< Ignore CR ? */
#define I_ICRNL(tty) (tty->termios.c_iflag & ICRNL)		/**< CR => NL ? */
#define I_INLCR(tty) (tty->termios.c_iflag & INLCR)		/**< NL => CR ? */

#define L_ISIG(tty) (tty->termios.c_lflag & ISIG)			/**< Signals enabled ? */
#define L_ECHOE(tty) (tty->termios.c_lflag & ECHOE)		/**< Echo erase ? */
#define L_ECHOCTL(tty) (tty->termios.c_lflag & ECHOCTL)		/**< Echo Ctrl ? */

#define QUIT_CHAR(tty) ((tty)->termios.c_cc[VQUIT])		/**< Quit char */
#define INTR_CHAR(tty) ((tty)->termios.c_cc[VINTR])		/**< Interrupt char */
#define SUSP_CHAR(tty) ((tty)->termios.c_cc[VSUSP])		/**< Suspend char */
#define ERASE_CHAR(tty) ((tty)->termios.c_cc[VERASE])	/**< Erase char */
#define ERASE_WORD(tty) ((tty)->termios.c_cc[VWERASE])	/**< Erase char */
#define EOF_CHAR(tty) ((tty)->termios.c_cc[VEOF])	/**< EOF char */

/**
 *  Indice du charactère precedent.
 */
#define PREV_CHAR_INDEX(tty) ((tty->p_end + MAX_INPUT - 1) % MAX_INPUT)
#define NEXT_CHAR_INDEX(tty) ((tty->p_end + 1) % MAX_INPUT)

static chardev_interfaces* tty_get_driver_interface(tty_struct_t *tty);

void tty_init() {

}

static void tty_delete_last_char(tty_struct_t *tty) {
	if (tty->p_end != tty->p_begin) {
		tty->p_end = PREV_CHAR_INDEX(tty);
		tty->driver->ops->write(tty, NULL, (unsigned char*)"\b \b", 3);
	}
}

void tty_insert_flip_char(tty_struct_t *tty, unsigned char c) {
	if (tty == NULL) return;

	// note: chaque driver doit fournir le write, par contre le put_char c'est
	// juste pour optimiser et éviter un appel de fonction... C'était peut être
	// pas nécessaire...

	if (c == '\r') {
		if (I_IGNCR(tty)) {
			return;
		}
		if (I_ICRNL(tty)) {
			c = '\n';
		}
	} else if (c == '\n' && I_INLCR(tty)) {
		c = '\r';
	} else if (c == EOF_CHAR(tty)) {
		c = EOF;
	}

	if (L_ISIG(tty)) {
		if (c == QUIT_CHAR(tty)) {
			sys_kill(tty->fg_process, SIGQUIT, NULL);
			return;
		}
		if (c == INTR_CHAR(tty)) {
			sys_kill(tty->fg_process, SIGINT, NULL);
			return;
		}
		if (c == SUSP_CHAR(tty)) {
			sys_kill(tty->fg_process, SIGTSTP, NULL);
			return;
		}
	}

	if (c == ERASE_CHAR(tty) && I_ECHO(tty) && I_CANON(tty)) {
		if (L_ECHOE(tty)) {
			tty_delete_last_char(tty);
		} else {
			if (tty->driver->ops->put_char != NULL) {
				tty->driver->ops->put_char(tty, c);
			} else {
				tty->driver->ops->write(tty, NULL, &c, 1);
			}
		}
	} else if (c == ERASE_WORD(tty) && I_ECHO(tty) && I_CANON(tty)) {
		// Supprime les espaces.
		while (tty->p_end != tty->p_begin && tty->buffer[PREV_CHAR_INDEX(tty)] == ' ') {
			tty_delete_last_char(tty);
		}
		// Supprime jusqu'au prochain espace.
		while (tty->p_end != tty->p_begin && tty->buffer[PREV_CHAR_INDEX(tty)] != ' ') {
			tty_delete_last_char(tty);
		}
	} else if (NEXT_CHAR_INDEX(tty) != tty->p_begin) {
		tty->buffer[tty->p_end] = c;
		tty->p_end = NEXT_CHAR_INDEX(tty);

		if (I_ECHO(tty)) {
			// Les 32 premiers caractères sont des caractères de ctrl.
			if (L_ECHOCTL(tty) && c <= 31) {
				if (c != 9 && c != 10) {
					unsigned char ch[3];
					ch[0] = '^';
					ch[1] = c + 0x40;
					ch[2] = '\0';
					tty->driver->ops->write(tty, NULL, ch, 2);
					return;
				}
			}

			if (tty->driver->ops->put_char != NULL) {
				tty->driver->ops->put_char(tty, c);
			} else {
				tty->driver->ops->write(tty, NULL, &c, 1);
			}
		}

		// Un caractère a été ajouté, on débloque la lecture.
		ksemV(tty->sem);
	}
}

tty_driver_t *alloc_tty_driver(int lines) {
	tty_driver_t *driver = kmalloc(sizeof(tty_driver_t));
	driver->num = lines;
	return driver;
}


int tty_register_driver(tty_driver_t *driver) {
	int n;
	driver->ttys = kmalloc(sizeof(tty_struct_t*) * driver->num);
	for (n = 0; n < driver->num; n++) {
		int len = strlen(driver->devfs_name);
		char *name = kmalloc(len + 5);
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
		driver->ttys[n]->sem = ksemget(SEM_NEW, SEM_CREATE);
		driver->ttys[n]->index = n;
		driver->ttys[n]->fg_process = 0;
		driver->ttys[n]->n_open = 0;
		register_chardev(name, tty_get_driver_interface(driver->ttys[n]));
	}
	return 0;
}

static ssize_t tty_write(open_file_descriptor *ofd, const void *buf, size_t count) {
	if (count <= 0) {
		return -2;
	}

	if((ofd->flags & O_ACCMODE) == O_RDONLY) {
		//errno = EBADF;
		return -1;
	}

	chardev_interfaces *di = ofd->i_fs_specific;
	tty_struct_t *t = (tty_struct_t *) di->custom_data;
	process_t *current_process = get_current_process();

	if (current_process->pid != t->fg_process) {
		klog("SIGTTOU venant du term %d de %d car != %d", t->index, current_process->pid, t->fg_process);

		sys_kill(current_process->pid, SIGTTOU, NULL);
	}

	return t->driver->ops->write(t, ofd, buf, count);
}

static ssize_t tty_read(open_file_descriptor *ofd, void *buf, size_t count) {
	if (count <= 0) {
		return -2;
	}

	char c;
	unsigned int j = 0;
	chardev_interfaces *di = ofd->i_fs_specific;
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
		// note: cette boucle peut en théorie être remplacée par un simple if.
		while (t->p_begin == t->p_end && !(ofd->flags & O_NONBLOCK)) {
			// Attend l'insertion d'un caractère dans le buffer.
			ksemP(t->sem);
		}

		// Lecture du dernier caractère inséré.
		c = t->buffer[PREV_CHAR_INDEX(t)];

		// On lit seulement si en mode non canonique ou si caractère qui flush.
		if (!I_CANON(t) || c == '\n' || c == '\r' || c == EOF) {
			while (j < count && t->p_begin != t->p_end) {
				c = t->buffer[t->p_begin];
				if (c != EOF) {
					((char*) buf)[j] = c;
					j++;
				}
				t->p_begin = (t->p_begin + 1) % MAX_INPUT;
			}
			break;
		} else {
			if (ofd->flags & O_NONBLOCK) {
				return -1;
			}
			// Attend l'insertion d'un autre caractère dans le buffer.
			ksemP(t->sem);
		}
	} while (1);

	return j;
}

static int tty_ioctl (open_file_descriptor *ofd, unsigned int request, void *data) {

	chardev_interfaces *di = ofd->i_fs_specific;
	tty_struct_t *t = (tty_struct_t *) di->custom_data;

	switch	(request) {
	case TCGETS:
		memcpy(data, &t->termios, sizeof(struct termios));
		return 0;
	case TCSETS:
		if (t->driver->ops->set_termios) {
			t->driver->ops->set_termios(data, &t->termios);
		} else {
			memcpy(&t->termios, data, sizeof(struct termios));
		}
		return 0;
	case TIOCGWINSZ:
		{struct winsize ws;
		ws.ws_row = 25;
		ws.ws_col = 80;
		//TODO: prendre les infos depuis le periphérique.
		memcpy(data, &ws, sizeof(struct winsize));}
		return 0;
	case TIOCSCTTY:
		{
		//XXX: Ce système de ctrl_tty est peut être pas top, à voir...
		process_t *current_process = get_current_process();
		current_process->ctrl_tty = strdup(ofd->pathname);
		}
		return 0;
	}

	return 0;
}

static int tty_close (open_file_descriptor *ofd) {
	chardev_interfaces *di = ofd->i_fs_specific;
	tty_struct_t *t = (tty_struct_t *) di->custom_data;

	if (t->n_open == 0) {
		return 1;
	}
	t->n_open--;
	//XXX :/ C'est vraiment n'imp en l'état.
	process_t *current_process = get_current_process();
	if (current_process->pid == t->fg_process) {
		t->fg_process = current_process->ppid;
	}
	return 0;
}

static int tty_open (open_file_descriptor *ofd) {
	chardev_interfaces *di = ofd->i_fs_specific;
	tty_struct_t *t = (tty_struct_t *) di->custom_data;

	if (t->n_open == 0) {
		process_t *current_process = get_current_process();
		if (current_process)
			t->fg_process = current_process->pid;
	}

	t->n_open++;
	return 0;
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
