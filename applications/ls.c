/**
 * @file ls.c
 *
 * @author TacOS developers 
 *
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
 * List files
 */

#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

void listdir(const char *path) {
	struct stat buf;
	DIR* dir = opendir(path);
	struct dirent* entry;
	if (dir != NULL) {
		char c;
		while((entry = readdir(dir))) {
			stat(entry->d_name, &buf);
			if (S_ISDIR(buf.st_mode)) {
				c = '/';
			} else {
				c = ' ';
			}
			printf("%s%c ", entry->d_name, c);
		}
		closedir(dir);
	} else {
		fprintf(stderr, "%s not found.\n", path);
	}
}

void list(const char *path) {
	struct stat buf;
	if (stat(path, &buf) == 0) {
		if (S_ISDIR(buf.st_mode)) {
			listdir(path);
		} else {
			printf("%s ", path);
		}
	} else {
		fprintf(stderr, "%s not found.\n", path);
	}
}

int main(int argc, char** argv)
{
	
	if (argc == 1) {
		list(getcwd(NULL, 0));
	} else {
		int i;
		for(i = 1; i < argc; i++) {
			list(argv[i]);
		}
	}
	printf("\n");
	return 0;
}

