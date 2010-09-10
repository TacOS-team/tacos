/**
 * @file tty.c
 *
 * @author TacOS developers 
 *
 * Maxime Cheramy <maxime81@gmail.com>
 * Nicolas Floquet <nicolasfloquet@gmail.com>
 * Benjamin Hautbois <bhautboi@gmail.com>
 * Ludovic Rigal <ludovic.rigal@gmail.com>
 * Simon Vernhes <simon@vernhes.eu>
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

/*
 * @file tty.c
 *
 *  Created on: 28 août 2010
 *      Author: Max
 */

#include "tty.h"
#include "ksem.h"
#include "kprocess.h"
#include "ksignal.h"
#include "kstdio.h"
#include <errno.h>

static terminal_t *active_tty = NULL;

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
		kprintf("Break!\n");
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
	 * TODO: Si un processus essaye d'écrire dans un tty alors qu'il n'est pas en foreground : SIGTTOU
	 */
	unsigned int i;

  if((ofd->flags & O_ACCMODE) == O_RDONLY) {
    errno = EBADF;
    return -1;
  }

	terminal_t *t = (terminal_t *) ofd->extra_data;
	for (i = 0; i < count; i++) {
		t->put_char(t->extra_data, ((char*) buf)[i]);
	}
	return count;
}

size_t tty_read(open_file_descriptor *ofd, void *buf, size_t count) {
	/*
	 * La lecture sur le tty correspond à essayer de lire des caractères.
	 * On bloque tant qu'il n'y a rien à lire (semaphore).
	 * TODO: Si un process essaye de lire sans être au premier plan => on lui envoie un signal SIGTTIN
	 */
	char c;
	unsigned int j = 0;
	terminal_t *t = (terminal_t *) ofd->extra_data;

  if((ofd->flags & O_ACCMODE) == O_WRONLY) {
    errno = EBADF;
    return -1;
  }

  if(ofd->flags & O_DIRECT) {
    errno = EINVAL;
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
			if (j == count)
				break;
		} else {
			ksemP(t->sem);
		}
	} while (1);

	return j;
}
