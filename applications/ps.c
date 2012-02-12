/**
 * @file ps.c
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
 * List processes
 */

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

void print_cmdline(int pid) {
	FILE* fd;
	char path[128];
	char c;
	sprintf(path,"/proc/%d/cmd_line",pid);
	
	fd = fopen(path, "r");
	if(fd != NULL)
	{
		c = fgetc(fd);
		while( c != EOF )
		{
		printf("%c",c);
		c = fgetc(fd);
		}
		fclose(fd);
	}	
}

void print_state(int pid) {
	FILE* fd;
	char c;
	char path[128];
	sprintf(path,"/proc/%d/state",pid);
	
	fd = fopen(path, "r");
	if(fd != NULL)
	{
		c = fgetc(fd);
		while( c != EOF )
		{
		printf("%c",c);
		c = fgetc(fd);
		}
		fclose(fd);
	}	
}


int main() { 
	DIR* dir = opendir("/proc");
	struct dirent* entry;
	int pid;
	
	if (dir != NULL) {
		while((entry = readdir(dir))) {
			pid = atoi(entry->d_name);
			printf("%d\t", pid);
			print_cmdline(pid);
			printf("\t");
			print_state(pid);
			printf("\n");
		}
		closedir(dir);
	}
	return 0;
}
