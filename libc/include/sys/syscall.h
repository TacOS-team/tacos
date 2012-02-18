/**
 * @file syscall.h
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
 * Liste des SYSCALL de TacOS.
 */

#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <sys/types.h>

#define SYS_EXIT 0
#define SYS_GETPID 1
#define SYS_GETPPID 2
#define SYS_OPEN 3
#define SYS_KILL 4
#define SYS_WRITE 5
#define SYS_READ 6
#define SYS_EXEC 7
#define SYS_SLEEP 8
#define SYS_SEMCTL 9
#define SYS_VIDEO_CTL 10
#define SYS_PROC 11
#define SYS_VMM 12
#define SYS_SEEK 13
#define SYS_HLT 14
#define SYS_SIGNAL 15
#define SYS_SIGPROCMASK 16
#define SYS_CLOSE 17
#define SYS_MKNOD 18
#define SYS_READDIR 19
#define SYS_MKDIR 20
#define SYS_SIGRET 21
#define SYS_SIGSUSPEND 22
#define SYS_IOCTL 23
#define SYS_GETCLOCK 24
#define SYS_GETDATE 25
#define SYS_FCNTL 26
#define SYS_DUMMY 27
#define SYS_STAT 28
#define SYS_UNLINK 29
#define SYS_DUP 30
#define SYS_WAITPID 31
#define SYS_RMDIR 32

#endif
