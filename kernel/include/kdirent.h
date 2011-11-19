/**
 * @file kdirent.c
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
#include <dirent.h>
#include <sys/stat.h>

SYSCALL_HANDLER2(sys_opendir, const char *name, int *ret);

#define ENOENT 2

SYSCALL_HANDLER3(sys_readdir, DIR *dir, struct dirent *entry, int *ret);
SYSCALL_HANDLER3(sys_mkdir, const char *pathname, mode_t mode, int *ret);

#endif
