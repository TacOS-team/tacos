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

#include "tty.h"
#include "ksem.h"
#include "kprocess.h"
#include "ksignal.h"
#include "kstdio.h"
#include <klog.h>
#include <kmalloc.h>
#include <fcntl.h>
#include <termios.h>

static terminal_t *terminals[10]; // "Temporaire"
static terminal_t *active_tty = NULL;

int tty_alloc() {
	int i = 0;
	while (terminals[i] && i < 10) {
		i++;
	}
  terminals[i] = kmalloc(sizeof(terminal_t));
	return i;
}

terminal_t * tty_get(int i) {
	return terminals[i];
}

void tty_init(terminal_t *t, process_t *process, void * extra_data, void(*put_char)(void*, char)) {
	t->echo = TRUE;
	t->canon = TRUE;
	t->p_begin = 0;
	t->p_end = 0;
	t->extra_data = extra_data;
	t->put_char = put_char;
	t->fg_process = process;
	uint8_t key;
	t->sem = ksemcreate_without_key(&key);
	ksemP(t->sem);
}

/**
 * @brief Fonction appelée par le driver clavier ou série pour ajouter un caractère.
 */
void tty_add_char(terminal_t *t, char c) {
	if (t == NULL) return;

	if (c == ('c' & 0x1F)) {
		kdebug("Break!");
		sys_kill(t->fg_process->pid, SIGINT, NULL);
	} else if (c == ('z' & 0x1F)) {
		sys_kill(t->fg_process->pid, SIGTSTP, NULL);
	} else if (c == '\b') {
		if (t->p_end != t->p_begin) {
			t->p_end--;
			if (t->echo) {
				t->put_char(t->extra_data, c);
			}
		}
	} else {
		if ((t->p_end + 1) % MAX_INPUT != t->p_begin) {
			t->buffer[t->p_end] = c;
			t->p_end = (t->p_end + 1) % MAX_INPUT;
			if (t->echo) {
				t->put_char(t->extra_data, c);
			}
			// Comment: Quelqu'un sait pourquoi ce bout de code est commenté ? 
			// S'il ne doit pas être là on supprime, sinon un petit commentaire 
			// pour expliquer ce qui déconne serait pas mal.
			//if (!t->canon || c == '\n') {
			ksemV(t->sem);
			//}
		}
	}
}

terminal_t * get_active_tty() {
    return active_tty;
}

void set_active_tty(terminal_t *tty) {
    active_tty = tty;
}

void tty_set_fg_process(terminal_t *tty, process_t *process) { //TODO : remplacer par un groupe.
    tty->fg_process = process;
}

size_t tty_write(open_file_descriptor *ofd, const void *buf, size_t count) {
	/*
	 * Permet d'écrire sur le terminal (écran ?)
	 */
	unsigned int i;
  if((ofd->flags & O_ACCMODE) == O_RDONLY) {
    //errno = EBADF;
    return -1;
  }

	int pid = get_pid();
	terminal_t *t = (terminal_t *) ofd->extra_data;

	if (get_process(pid) != t->fg_process) {
		sys_kill(pid, SIGTTOU, NULL);
	}

	for (i = 0; i < count; i++) {
		t->put_char(t->extra_data, ((char*) buf)[i]);
	}
	return count;
}

size_t tty_read(open_file_descriptor *ofd, void *buf, size_t count) {
	/*
	 * La lecture sur le tty correspond à essayer de lire des caractères.
	 * On bloque tant qu'il n'y a rien à lire (semaphore).
	 */
	char c;
	unsigned int j = 0;
	terminal_t *t = (terminal_t *) ofd->extra_data;
	int pid = get_pid();

	if (get_process(pid) != t->fg_process) {
		sys_kill(pid, SIGTTIN, NULL);
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
		if (t->p_begin == t->p_end)
			ksemP(t->sem);

		c = t->buffer[(t->p_end + MAX_INPUT - 1) % MAX_INPUT];

		if (!t->canon || c == '\n') {
			while (j < count && t->p_begin < t->p_end) {
				((char*) buf)[j] = t->buffer[t->p_begin];
				t->p_begin = (t->p_begin + 1) % MAX_INPUT;
				j++;
			}
			break;
		} else {
			ksemP(t->sem);
		}
	} while (1);

	return j;
}

int tty_ioctl (open_file_descriptor *ofd, unsigned int request, void * data) {
	terminal_t *t = (terminal_t*) ofd->extra_data;

	if (request == TCGETS) {
		struct termios *c = data;
		c->c_lflag = 0;
		c->c_lflag |= t->echo ? ECHO : 0;
		c->c_lflag |= t->canon ? ICANON : 0;
	} else if (request == TCSETS) {
		struct termios *c = data;
		t->echo = c->c_lflag & ECHO;
		t->canon = c->c_lflag & ICANON;
	} 
	return 0;
}
