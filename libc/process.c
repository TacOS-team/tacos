/**
 * @file process.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012, 2013 - TacOS developers.
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

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h> 
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>
#include <fcntl.h>

#define GET_PROCESS 0 /**< Action pour obtenir les infos d'un process. */
#define GET_PROCESS_LIST 1 /**< Action pour obtenir la liste. */

extern char **environ;

int exec_elf(char* cmdline)
{
	int ret;
	syscall(SYS_EXEC, (uint32_t)cmdline, (uint32_t)environ, (uint32_t)&ret);
	return ret;
}
