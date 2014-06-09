/**
 * @file cat.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2014 TacOS developers.
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
#include <unistd.h>
#include <fcntl.h>

void cat(int fd) {
	char buf[80];
	int c;

	while ((c = read(fd, buf, sizeof(buf))) > 0) {
		write(1, buf, c); // écrit sur stdout.
	}
}

int main(int argc, char **argv)
{
	int i;

	if (argc == 1) {
		cat(0);
	}

	for (i = 1; i < argc; i++) {
		int fd = open(argv[i], O_RDONLY);
		if (fd >= 0) {
			cat(fd);
		} else {
			fprintf(stderr, "%s: file not found.\n", argv[i]);
		}
	}
	return 0;
}
