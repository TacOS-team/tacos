/**
 * @file klibc/limits.h
 *
 * @author TacOS developers 
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
 * @brief Valeurs extrèmes pour les types de base.
 */

#define CHAR_BIT 8 /**< Nombre de bits pour un char. */
#define INT_MAX 2147483647 /**< Valeur maximale d'un int signé. */
#define INT_MIN (-INT_MAX - 1) /**< Valeur minimale d'un int signé. */
#define UINT_MAX 4294967295U /**< Valeur maximale d'un int non signé. */
#define LONG_MAX 2147483647L /**< Valeur maximale d'un long signé. */
#define LONG_MIN (-LONG_MAX - 1L) /**< Valeur minimale d'un long signé. */
