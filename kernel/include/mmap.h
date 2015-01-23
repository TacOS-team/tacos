/**
 * @file mmap.h
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
 * Gestion des mmap.
 */

#ifndef _MMAP_H_
#define _MMAP_H_

#include <ksyscall.h>
#include <types.h>

struct mmap_data {
	void *addr;
	size_t length;
	int prot;
	int flags;
	int fd;
	off_t offset;
};

SYSCALL_HANDLER2(sys_mmap, struct mmap_data* data, void **ret);

#endif
