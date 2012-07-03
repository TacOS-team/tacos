/**
 * @file chmod.c
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
 * Change les droits.
 */

#include <dirent.h>
#include <stdio.h>

mode_t convert(char *droits) {
	mode_t m = 0;
	while (*droits) {
		if (*droits < '0' || *droits > '7') {
			return -1;
		}
		m = m * 8 + *droits - '0';
		droits++;
	}
	return m;
}

int main(int argc, char** argv)
{
	int i;
	
	if (argc <= 2) {
		fprintf(stderr, "usage: %s droits fichier\n", argv[0]);
	} else {
		mode_t droits = convert(argv[1]);
		if (droits != (mode_t)(-1)) {
			for(i = 2; i < argc; i++) {
				if (chmod(argv[i], droits) != 0) {
					fprintf(stderr, "Le fichier %s n'existe pas.\n", argv[i]);
				} else {
					printf("Changement des droits de %s, ok.\n", argv[i]);
				}
			}
		}
	}
	return 0;
}

