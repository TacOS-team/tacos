/**
 * @file dirent.c
 *
 * @author TacOS developers 
 *
 * Maxime Cheramy <maxime81@gmail.com>
 * Nicolas Floquet <nicolasfloquet@gmail.com>
 * Benjamin Hautbois <bhautboi@gmail.com>
 * Ludovic Rigal <ludovic.rigal@gmail.com>
 * Simon Vernhes <simon@vernhes.eu>
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
 * Définition de DIR et struct dirent.
 */

#include <dirent.h>
#include <stdlib.h>
#include <syscall.h>

DIR* opendir(const char* dirname) {
	DIR* dir = malloc(sizeof(DIR));
	int ret;
	syscall(SYS_OPENDIR, (uint32_t) dir, (uint32_t) dirname, (uint32_t) &ret);
	if (ret == 0) {
		return dir;
	}
	return NULL;
}

struct dirent* readdir(DIR* dirp) {
	int ret;
	struct dirent *entry = malloc(sizeof(struct dirent));
	syscall(SYS_READDIR, (uint32_t) dirp, (uint32_t) entry, (uint32_t) &ret);
	if (ret == 0) {
		dirp->iter++;
		return entry;
	} else {
		free(entry);
		return NULL;
	}
}

int closedir(DIR* dirp) {
	if (dirp) {
		free(dirp);
		return 0;
	}
	return 1;
}
