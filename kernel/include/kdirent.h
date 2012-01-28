/**
 * @file kdirent.h
 *
 * @author TacOS developers 
 *
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

#ifndef _KDIRENT_H
#define _KDIRENT_H

#include <ksyscall.h>
#include <types.h>
#include <kstat.h>

#define NAME_MAX 256

struct dirent {
	uint32_t  d_ino;
	uint16_t  d_reclen;
	uint8_t   d_type;
	char      d_name[NAME_MAX];
};

#define ENOENT 2
#define ENOTDIR 3
#define EEXIST 4

SYSCALL_HANDLER3(sys_readdir, int fd, char *entries, size_t *size);
SYSCALL_HANDLER3(sys_mkdir, const char *pathname, mode_t mode, int *ret);

#endif
