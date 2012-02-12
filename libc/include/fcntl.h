/**
 * @file fcntl.h
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
 * Description de ce que fait le fichier
 */

#ifndef _FCNTL_H_
#define _FCNTL_H_

/**
 * @file fcntl.h
 */

// Define pour open.

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

int open(const char *pathname, int flags);
int close(int id);
int fcntl(int fd, unsigned int request, void * data);
//int open(const char *pathname, int flags, mode_t mode);

//int creat(const char *pathname, mode_t mode);


#endif
