/**
 * @file process.c
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

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h> 
#include <unistd.h>
#include <sys/syscall.h>
#include <process.h>
#include <elf.h>
#include <string.h>
#include <fcntl.h>

#define GET_PROCESS 0 /**< Action pour obtenir les infos d'un process. */
#define GET_PROCESS_LIST 1 /**< Action pour obtenir la liste. */

extern char **environ;

//void exec(void* prog, char* name, int orphan)
//{
//	process_init_data_t init_data;
//	
//	init_data.name	= name;
//	init_data.stack_size = 0x1000;
//	init_data.priority = 0;
//	init_data.args = "fajitas bonitas";
//	init_data.envp = environ;
//
//	init_data.data = prog;
//
//	init_data.mem_size = 0;
//	init_data.entry_point = 0;
//	
//	init_data.ppid = orphan?0:getpid();
//	
//	syscall(SYS_EXEC, (uint32_t)NULL, (uint32_t)&init_data, 1);
//}

int exec_elf(char* cmdline, int orphan)
{	
	char* execpath = strdup(cmdline);
	int ret = -1;
	int offset = 0;
	
	while(execpath[offset] != ' ' && execpath[offset] != '\0')
		offset++;
	
	execpath[offset] = '\0';
	
	struct stat buf;
	stat(execpath, &buf);

	if (!S_ISREG(buf.st_mode) || !(S_IXUSR & buf.st_mode)) {
		return -1;
	}

	int fd = open(execpath, O_RDONLY);
	
	process_init_data_t init_data;
	
	if(fd != -1)
	{
		ret = 0;
			
		init_data.name	= execpath;
		init_data.stack_size = 0x1000;
		init_data.priority = 0;
		
		init_data.args = cmdline;
		init_data.envp = environ;
		
		init_data.mem_size = elf_size(fd);
		init_data.data = calloc(init_data.mem_size, 1);
		init_data.entry_point = load_elf(fd, init_data.data);
		
		init_data.file = load_elf_file(fd);

		init_data.ppid = orphan?0:getpid();
		init_data.exec_type = EXEC_ELF;

		syscall(SYS_EXEC, (uint32_t)&init_data, (uint32_t)&ret, (uint32_t)NULL);
		
		free(init_data.data);
	}
	free(execpath);
	return ret;
}
