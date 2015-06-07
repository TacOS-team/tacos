/**
 * @file process_types.h
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
 * Types pour les process (header à part pour éviter trop de dépendances 
 * inutiles).
 */

#ifndef _PROCESS_TYPES_H_
#define _PROCESS_TYPES_H_

typedef struct
{
	uint32_t eax, ecx, edx, ebx;
	uint32_t esp, kesp, ebp, esi, edi;
	uint32_t eip, eflags;
	uint16_t cs, ss, kss, ds, es, fs, gs;
	uint32_t cr3;
} regs_t;

#endif
