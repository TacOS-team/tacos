/**
 * @file wait.c
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
 * Description de ce que fait le fichier
 */

#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>

#include <stdio.h>

pid_t wait(int *status __attribute__ ((unused))) {
//XXX: devrait juste faire : waitpid(-1, &status, 0);
	sigset_t set;
	sigfillset(&set);
	sigdelset(&set, SIGCHLD);
	sigsuspend(&set);
	return -1;
}

void waitpid(pid_t pid) {
	syscall(SYS_WAITPID, (uint32_t) pid, (uint32_t)NULL, (uint32_t)NULL);
}
