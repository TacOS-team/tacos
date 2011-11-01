/**
 * @file console.h
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

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "types.h"
#include "tty.h"

struct console_t {
	bool used;
	unsigned int n_page;
	unsigned int cur_x;
	unsigned int cur_y;
	bool disp_cur;
	char attr;
	unsigned int lines;
	unsigned int cols;
    terminal_t *tty;
};

/**
 * @brief Passe une console au premier plan.
 *
 * @param n Numero de la console.
 */
void focus_console(int n);

/**
 * @brief Libère une console.
 *
 * @param n Numero de la console.
 */
void free_console(int n);

/**
 * @brief Initialise les consoles à "inutilisée".
 */
void init_console();

/**
 * @brief Trouve une console libre.
 */
int get_available_console(terminal_t *tty);

/**
 * @brief Affiche un caractère. Supporte les caractères ANSI.
 *
 * @param n Numero de la console.
 * @param c Caractère à afficher.
 */
void kputchar(int n, char c);

#endif
