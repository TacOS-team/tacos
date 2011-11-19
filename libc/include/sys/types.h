/**
 * @file sys/types.h
 *
 * @author TacOS developers 
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
 */

#ifndef _SYS_TYPES_H_
#define _SYS_TYPES_H_

#include <stdbool.h>

typedef int pid_t;

#ifndef _TYPES_UINTX_T_
#define _TYPES_UINTX_T_
typedef unsigned char uint8_t;  /* 8 bits non signé */
typedef unsigned short int uint16_t;  /* 16 bits non signé */
typedef unsigned long int uint32_t;  /* 32 bits non signé */
typedef unsigned long long int uint64_t;  /* 64 bits non signé */
typedef uint32_t vaddr_t;
#endif

#ifndef _TYPES_SIZE_T_
#define _TYPES_SIZE_T_
typedef uint32_t size_t;
typedef int ssize_t;
#endif

typedef unsigned long long int off_t;

#define NULL ((void*)0)

#endif
