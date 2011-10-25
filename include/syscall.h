/**
 * @file syscall.h
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

#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <types.h>

#define SYS_EXIT 0
#define SYS_GETPID 1
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
#define SYS_OPENDIR 18
#define SYS_READDIR 19
#define SYS_MKDIR 20
#define SYS_SIGRET 21
#define SYS_SIGSUSPEND 22
#define SYS_IOCTL 23

void syscall(uint32_t func, uint32_t param1, uint32_t param2, uint32_t param3);

#endif
