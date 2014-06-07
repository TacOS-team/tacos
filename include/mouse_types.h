/**
 * @file mouse_types.h
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
 * @brief Defines types used by the mouse driver.
 */

#ifndef _MOUSE_TYPES_H_
#define _MOUSE_TYPES_H_

/**
 * @brief Structure pour réprésenter l'état de la souris.
 *
 * @struct mousestate_t
 */
typedef struct mousestate {
	unsigned int x : 13; /**< Coordonnée horizontale du curseur. */
	unsigned int y : 13; /**< Coordonnée verticale du curseur. */
	unsigned int b1 : 1; /**< Etat du bouton 1. */
	unsigned int b2 : 1; /**< Etat du bouton 2. */
	unsigned int b3 : 1; /**< Etat du bouton 3. */
	unsigned int b4 : 1; /**< Etat du bouton 4. */
	unsigned int b5 : 1; /**< Etat du bouton 5. */
	unsigned int b6 : 1; /**< Etat du bouton 6. */
} mousestate_t;

enum mouse_req_codes { SETRES };

struct mouse_setres_req {
	int width;
	int height;
};

#endif
