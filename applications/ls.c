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

int main(int argc, char** argv)
{
	int i;
	DIR* dir;
	struct dirent* entry;
	
	if (argc == 1) {
		dir = opendir(getcwd(NULL, 0));
		if (dir != NULL) {
			while((entry = readdir(dir)))
				printf("%s ", entry->d_name);
			printf("\n");
			closedir(dir);
		}
	} else {
		for(i = 1; i < argc; i++) {
			dir = opendir(argv[i]);
			if (dir != NULL) {
				while((entry = readdir(dir)))
					printf("%s ", entry->d_name);
				closedir(dir);
			} else {
				fprintf(stderr, "%s not found.\n", argv[i]);
			}
		}
	}
	return 0;
}

