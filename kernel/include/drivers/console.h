/**
 * @file console.h
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
 * @brief Console virtuelle (Fn)
 */

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <types.h>
#include <tty.h>

/**
 * @brief Structure qui contient les informations d'une console.
 */
struct console_t {
	bool used;						/**< Console activée. */
	unsigned int cur_x;		/**< Position horizontale du curseur. */
	unsigned int cur_y;		/**< Position verticale du curseur. */
	bool disp_cur;				/**< Curseur visible. */
	char attr;						/**< Attribut actuel pour l'affichage de caractères. */
	unsigned int lines;		/**< Nombre de lignes. */
	unsigned int cols;		/**< Nombre de colonnes. */

	bool escape_char;
	bool ansi_escape_code;
	bool ansi_second_val;
	int val;
	int val2;
	int bright;
};

/**
 * @brief Passe une console au premier plan.
 *
 * @param n Numero de la console.
 */
void focus_console(int n);

/**
 * @brief Initialise les consoles à "inutilisée".
 */
void console_init();

/**
 * @brief Retourne le tty_struct_t qui correspond à la console au premier plan.
 */
tty_struct_t* get_active_terminal();

#endif
