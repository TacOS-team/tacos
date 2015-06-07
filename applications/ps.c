/**
 * @file ps.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2015 TacOS developers.
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
#include <string.h>
#include <sys/stat.h>

static const int MAX_CONTENT_LENGTH = 128;
static const int MIN_NAME_LENGTH    = 30;

void print_headers() {
	printf("PID\tPPID\tNAME");
	int length = 0;
	for (length=0; length < MIN_NAME_LENGTH - 2; ++length) {
		printf(" ");
	}
	printf("STATE\n");
}

void print_info(int pid, char * info_name, int min_length) {
	FILE* fd;
	char path[MAX_CONTENT_LENGTH];
	char c;
	sprintf(path, "/proc/%d/%s", pid, info_name);
	
	fd = fopen(path, "r");
	if (fd != NULL) {
		c = fgetc(fd);
		int length = 0;
		while (c != EOF) {
			printf("%c", c);
			c = fgetc(fd);
			++length;
		}
		while (length++ < min_length) {
			printf(" ");
		}
		fclose(fd);
	}
}

void print_cmdline(int pid) {
	print_info(pid, "cmd_line", MIN_NAME_LENGTH);
}

void print_ppid(int pid) {
	print_info(pid, "ppid", 0);
}

void print_state(int pid) {
	print_info(pid, "state", 11);
}


int main() { 
	DIR* dir = opendir("/proc");
	struct dirent* entry;
	struct stat buf;
	char filepath[15];
	int pid;
	
	if (dir != NULL) {
		print_headers();
		while ((entry = readdir(dir))) {
			strcpy(filepath, "/proc/");
			strcpy(filepath + 6, entry->d_name);
			if (lstat(filepath, &buf) == 0) {
				if (!S_ISDIR(buf.st_mode)) {
					continue;
				}
			}
			pid = atoi(entry->d_name);
			printf("%d\t", pid);
			print_ppid(pid);
			printf("\t");
			print_cmdline(pid);
			printf("\t");
			print_state(pid);
			printf("\n");
		}
		closedir(dir);
	}
	return 0;
}
