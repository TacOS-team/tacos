/**
 * @file kmalloc.h
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
 * @brief Allocateur mémoire en kernel space.
 */

#ifndef _K_MALLOC_H_
#define _K_MALLOC_H_

#include <types.h>

void init_kmalloc();

/**
 * @brief Allocation de size octets.
 *
 * Allocation de size octets en kernel space.
 *
 * @param size Taille à allouer.
 *
 * @return un pointeur vers la zone allouée.
 */
void *kmalloc(size_t size);

/**
 * Libération d'une zone allouée.
 *
 * @param pointeur vers la zone allouée.
 *
 * @return 0 en cas de succès.
 */
int kfree(void *p);

void *kmalloc_one_aligned_page();

#endif

