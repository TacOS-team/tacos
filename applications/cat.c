/**
 * @file cat.c
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
 * Concatenate
 */

#include <stdio.h>

int main(int argc, char **argv)
{
	int i;
	FILE *fd;
	char c;

	for (i = 1; i < argc; i++) {
		fd = NULL;
		fd = fopen(argv[i], "r");
		if (fd != NULL) {
			c = fgetc(fd);
			while (c != EOF) {
				printf("%c", c);
				c = fgetc(fd);
			}

			//fclose(fd);
		} else {
			printf("%s: file not found.\n", argv[i]);
		}
		printf("\n");
	}
	return 0;
}
