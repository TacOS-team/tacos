/**
 * @file errno.h
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

#ifndef _ERRNO_H_
#define _ERRNO_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

/* man 2 write - à éventuellement reclasser dans un ordre moins bourrin
 * ou copier /usr/include/asm-generic/errno-base.h */
#define EAGAIN    1 
#define EBADF     2
#define EFAULT    3
#define EFBIG     4
#define EINTR     5
#define EINVAL    6
#define EIO       7
#define ENOSPC    8
#define EPIPE     9
#define ERANGE    10

extern int errno;

__END_DECLS

#endif //_ERRNO_H_
