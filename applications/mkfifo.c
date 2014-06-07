/**
 * @file mkfifo.c
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
 * List files
 */

#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	int i;
	struct stat st;
	
	if (argc == 1) {
		fprintf(stderr, "%s: opérande manquant\n", argv[0]);
	} else {
		for(i = 1; i < argc; i++) {
			if (stat(argv[i], &st)) {
				mknod(argv[i], S_IFIFO|0666, 0);
			} else {
				fprintf(stderr, "Le fichier %s existe déjà.\n", argv[i]);
			}
		}
	}
	return 0;
}

