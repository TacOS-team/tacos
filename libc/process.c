/**
 * @file process.c
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

#include <types.h>
#include <stdlib.h>
#include <stdio.h> 
#include <unistd.h>
#include <sys/syscall.h>
#include <process.h>
#include <elf.h>
#include <string.h>

#define GET_PROCESS 0
#define GET_PROCESS_LIST 1


void exit(uint32_t value)
{ 
	syscall(SYS_EXIT,value,0,0);
	while(1); // Pour ne pas continuer à executer n'importe quoi alors que le processus est sensé être arrété
}

uint32_t get_pid()
{
	int pid;
	syscall(SYS_GETPID,(uint32_t)&pid, 0, 0);
	return pid;
}

void exec(paddr_t prog, char* name, int orphan)
{
	process_init_data_t init_data;
	
	init_data.name	= name;
	init_data.stack_size = 0x1000;
	init_data.priority = 0;
	init_data.args = "fajitas bonitas";

	init_data.data = (void *) prog;

	init_data.mem_size = 0;
	init_data.entry_point = 0;
	
	init_data.ppid = orphan?0:get_pid();
	
	syscall(SYS_EXEC, (uint32_t)NULL, (uint32_t)&init_data, 1);
}

int exec_elf(char* cmdline, int orphan)
{	
	char* execpath = strdup(cmdline);
	int ret = -1;
	int offset = 0;
	
	while(execpath[offset] != ' ' && execpath[offset] != '\0')
		offset++;
	
	execpath[offset] = '\0';
	
	FILE *fd = fopen(execpath, "r");
	
	process_init_data_t init_data;
	
	if(fd != NULL)
	{
		ret = 0;
			
		init_data.name	= execpath;
		init_data.stack_size = 0x1000;
		init_data.priority = 0;
		
		init_data.args = cmdline;
		
		init_data.mem_size = elf_size(fd);
		init_data.data = malloc(init_data.mem_size);
		init_data.entry_point = load_elf(fd, init_data.data);

		init_data.ppid = orphan?0:get_pid();
		init_data.exec_type = EXEC_ELF;

		syscall(SYS_EXEC, (uint32_t)&init_data, (uint32_t)NULL, (uint32_t)NULL);
		
		free(init_data.data);
	}
	return ret;
}

process_t* get_process(int pid)
{
	process_t* temp;
	syscall(SYS_PROC,(uint32_t)GET_PROCESS, (uint32_t)pid, (uint32_t) &temp);
	return temp;
}

process_t* get_process_list(uint32_t action)
{
	process_t* temp;
	syscall(SYS_PROC,(uint32_t)GET_PROCESS_LIST, (uint32_t)action, (uint32_t) &temp);
	return temp;
}
