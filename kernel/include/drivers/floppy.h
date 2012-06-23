/**
 * @file floppy.h
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
 * Floppy driver
 */

#ifndef _FLOPPY_H_
#define _FLOPPY_H_

#include <types.h>

/**
 * @brief Fonction de détection des lecteur floppy présents
 */
void floppy_detect_drives();

/** 
 * @brief Retourne le numero de version du controleur disquette
 * @return numero de version
 */
uint8_t floppy_get_version();

/**
 * @brief Initilise le driver floppy
 */
int init_floppy();


#endif
