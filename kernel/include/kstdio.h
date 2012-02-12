/**
 * @file kstdio.h
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012 - TacOS developers.
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

#ifndef _KSTDIO_H_
#define _KSTDIO_H_

/** 
 * @brief Affiche un message sur l'écran.
 *
 * Affiche un message sur l'écran. Attention, il n'y a pas d'appel système de
 * fait !
 *
 * @see printf
 *	@see fprintf
 *	@see sprintf
 *	@see snprintf
 *	@see vprintf
 *	@see vfprintf
 *	@see vsprintf
 *	@see vsnprintf
 * 
 * @param format chaîne de format précisant le format de conversion pour la
 * sortie.
 * @param ... liste variable d'arguments à afficher.
 */
void kprintf(const char *format, ...);

#endif
