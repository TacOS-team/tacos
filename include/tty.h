/**
 * @file tty.h
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

#ifndef _TTY_H
#define _TTY_H

#include <types.h>
#include <kfcntl.h>
#include <kprocess.h>

#define MAX_INPUT 255

typedef struct _terminal_t {
	bool echo;
	bool canon;
	// foreground_process; // Groupe de processus au premier plan (qui a le droit d'afficher des choses)
	process_t* fg_process;
	void (*put_char)(void*, char); // Fonction utilisée lors d'un affichage donc vers l'écran ou la ligne série.
	char buffer[MAX_INPUT];
	unsigned int p_begin;
	unsigned int p_end;
	void *extra_data;
	int sem;
} terminal_t;

int tty_alloc();
terminal_t * tty_get(int i);

void tty_init(terminal_t *term, process_t *process, void *extra_data, void (*put_char)(void*, char));

void tty_add_char(terminal_t *term, char c); // Fonction appelée par le driver clavier ou série pour ajouter un caractère.

terminal_t * get_active_tty();
void set_active_tty(terminal_t *tty);
void tty_set_fg_process(terminal_t *tty, process_t *process);

size_t tty_write(open_file_descriptor *ofd, const void *buf, size_t count);
size_t tty_read(open_file_descriptor *ofd, void *buf, size_t count);
int tty_ioctl (unsigned int request, void *data);

#endif
