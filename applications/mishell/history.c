/**
 * @file history.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2014 TacOS developers.
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
 * Gestion de l'historique pour readline. Inspiré de l'interface de GNU History.
 */

#include "history.h"
#include <stdlib.h>
#include <string.h>


static int history_size;
static int history_length;
static int history_offset;
static HIST_ENTRY* the_history;

void using_history() {
	history_offset = history_length;
}

void add_history(const char *str) {
	if (str[0] == '\0') {
		return;
	}
	if (history_length == history_size) {
		history_size += 100;
		if (history_size > 0) {
			the_history = realloc(the_history, history_size * sizeof(HIST_ENTRY));
		} else {
			the_history = malloc(history_size * sizeof(HIST_ENTRY));
		}
	}
	the_history[history_length++].cmd = strdup(str);
}

void clear_history() {
	while (history_length) {
		free(the_history[--history_length].cmd);
	}
	history_offset = 0;
}

int where_history(void) {
	return history_offset;
}

int history_set_pos(int pos) {
	if (pos >= 0 && pos < history_length) {
		history_offset = pos;
		return 0;
	}
	return -1;
}

HIST_ENTRY * current_history(void) {
	return history_get(history_offset);
}

HIST_ENTRY* history_get(int offset) {
	return &(the_history[offset]);
}

HIST_ENTRY * previous_history(void) {
	if (history_offset > 0) {
		return &(the_history[--history_offset]);
	}
	return NULL;
}

HIST_ENTRY * next_history(void) {
	if (history_offset < history_length - 1) {
		return &(the_history[++history_offset]);
	}
	history_offset = history_length;
	return NULL;
}
