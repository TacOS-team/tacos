/**
 * @file fopen.h
 *
 * @author TacOS developers 
 *
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
 * Description de ce que fait le fichier
 */

#ifndef _FOPEN_H_
#define _FOPEN_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

/**
 * @file fopen.h
 */

/** 
 * @brief Conversion de modes de lecture/ecriture/ajout...
 *
 * Converti depuis la forme "r+" par exemple en un entier 
 * formé de la superposition de plusieurs masques.
 * 
 * @param mode le mode présenté sous la forme d'une chaine de caractère.
 * 
 * @return un entier contenant les mêmes informations.
 */
int convert_flags(const char *mode);

__END_DECLS

#endif
