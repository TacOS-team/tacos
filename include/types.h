/**
 * @file types.h
 *
 * @author TacOS developers 
 *
 * Maxime Cheramy <maxime81@gmail.com>
 * Nicolas Floquet <nicolasfloquet@gmail.com>
 * Benjamin Hautbois <bhautboi@gmail.com>
 * Ludovic Rigal <ludovic.rigal@gmail.com>
 * Simon Vernhes <simon@vernhes.eu>
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

#ifndef _TYPES_H_
#define _TYPES_H_

typedef unsigned char uint8_t;  /* 8 bits non signé */
typedef unsigned short int uint16_t;  /* 16 bits non signé */
typedef unsigned long int uint32_t;  /* 32 bits non signé */
typedef unsigned long long int uint64_t;  /* 64 bits non signé */

typedef uint32_t size_t;
typedef uint32_t paddr_t;
typedef uint32_t vaddr_t;
typedef uint32_t mode_t;

typedef int ssize_t;

typedef enum {FALSE=0, TRUE} bool;

/* Pointeur null */
#define NULL ((void*)0)

#endif
