/**
 * @file mkdir.c
 *
 * @author TacOS developers 
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
 * Crée un dossier.
 */

#include <dirent.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char** argv)
{
	int i;
	
	if (argc == 1) {
		fprintf(stderr, "%s: opérande manquant\n", argv[0]);
	} else {
		for(i = 1; i < argc; i++) {
			if (mkdir(argv[i], 0)) {
				fprintf(stderr, "%s: impossible de créer le répertoire \"%s\"", argv[0], argv[i]);

				switch(errno) {
					case ENOENT:
						fprintf(stderr, ": Aucun fichier ou dossier de ce type\n");
						break;
					case EEXIST:
						fprintf(stderr, ": Le fichier existe\n");
						break;
					default:
						fprintf(stderr, "\n");
				}
			}
		}
	}
	return 0;
}

