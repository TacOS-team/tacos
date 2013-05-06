/**
 * @file chown.c
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
 * Change proprio et groupe.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	int i;
	
	if (argc <= 2) {
		fprintf(stderr, "usage: %s uid:gid fichier\n", argv[0]);
	} else {
		uid_t uid = atoi(argv[1]); // TODO: gérer gid
		if (uid != (uid_t)(-1)) {
			for(i = 2; i < argc; i++) {
				if (chown(argv[i], uid, -1) != 0) {
					fprintf(stderr, "Le fichier %s n'existe pas.\n", argv[i]);
				} else {
					printf("Changement des droits de %s, ok.\n", argv[i]);
				}
			}
		}
	}
	return 0;
}

