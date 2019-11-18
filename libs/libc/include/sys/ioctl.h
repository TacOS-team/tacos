/**
 * @file ioctl.h
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
 */

#ifndef _IOCTL_H
#define _IOCTL_H

#include <sys/cdefs.h>

__BEGIN_DECLS

#define TCGETS 1
#define TCSETS 2
#define TIOCGWINSZ 3
#define TIOCSCTTY 4

/**
 * @brief Structure qui contient la taille d'une fenetre.
 */
struct winsize {
	unsigned short ws_row; /**< Nombre de lignes. */
	unsigned short ws_col; /**< Nombre de colonnes. */
};

int ioctl(int d, unsigned int request, void *data);

__END_DECLS

#endif
