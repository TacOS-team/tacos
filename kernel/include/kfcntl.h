/**
 * @file kfcntl.h
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
 * Description de ce que fait le fichier
 */

#ifndef _KFCNTL_H
#define _KFCNTL_H

/**
 * @file kfcntl.h
 */

#include <types.h>
#include <fd_types.h>
#include <kprocess.h>
#include <ksyscall.h>

// Defines pour open.

#define O_ACCMODE 00000003
#define O_RDONLY  00000000
#define O_WRONLY  00000001
#define O_RDWR    00000002
#ifndef O_CREAT
#define O_CREAT      00000100 /* not fcntl */
#endif
#ifndef O_EXCL
#define O_EXCL    00000200 /* not fcntl */
#endif
#ifndef O_NOCTTY
#define O_NOCTTY  00000400 /* not fcntl */
#endif
#ifndef O_TRUNC
#define O_TRUNC      00001000 /* not fcntl */
#endif
#ifndef O_APPEND
#define O_APPEND  00002000
#endif
#ifndef O_NONBLOCK
#define O_NONBLOCK   00004000
#endif
#ifndef O_SYNC
#define O_SYNC    00010000
#endif
#ifndef FASYNC
#define FASYNC    00020000 /* fcntl, for BSD compatibility */
#endif
#ifndef O_DIRECT
#define O_DIRECT  00040000 /* direct disk access hint */
#endif
#ifndef O_LARGEFILE
#define O_LARGEFILE  00100000
#endif
#ifndef O_DIRECTORY
#define O_DIRECTORY  00200000 /* must be a directory */
#endif
#ifndef O_NOFOLLOW
#define O_NOFOLLOW   00400000 /* don't follow links */
#endif
#ifndef O_NOATIME
#define O_NOATIME 01000000
#endif
#ifndef O_CLOEXEC
#define O_CLOEXEC 02000000 /* set close_on_exec */
#endif
#ifndef O_NDELAY
#define O_NDELAY  O_NONBLOCK
#endif

#define F_SETFL	4
#define F_GETFL	5

/** 
 * @brief Initialise les descripteurs de fichiers standards.
 * 
 *	Initialise les descripteurs de fichiers standards.
 *
 * @param new_proc Processus à initialiser.
 */
void init_stdfd(process_t *new_proc);

/**
 *	@brief Ferme tous les file descriptors ouverts.
 */
void close_all_fd();

SYSCALL_HANDLER3(sys_open, uint32_t fd_id, char *path , uint32_t flags);
SYSCALL_HANDLER2(sys_close, uint32_t fd_id, uint32_t* ret);
SYSCALL_HANDLER3(sys_fcntl, int *fd_id, unsigned int request, void * data);

#endif
