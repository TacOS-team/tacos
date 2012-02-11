/**
 * @file dirent.c
 *
 * @author TacOS developers 
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
 * @brief Fonctions pour faciliter la gestion des dossiers.
 */

#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <stdio.h>

int mkdir(const char *pathname, mode_t mode) {
	int ret;
	if (pathname[0] != '/') {
		char * absolutepath = get_absolute_path(pathname);
		syscall(SYS_MKDIR, (uint32_t) absolutepath, (uint32_t) mode, (uint32_t) &ret);
		free(absolutepath);
	} else {
		syscall(SYS_MKDIR, (uint32_t) pathname, (uint32_t) mode, (uint32_t) &ret);
	}
	return ret;
}

DIR* opendir(const char* dirname) {
	struct stat buf;
	int ret;
	if (dirname[0] != '/') {
		char * absolutepath = get_absolute_path(dirname);
		syscall(SYS_STAT, (uint32_t) absolutepath, (uint32_t) &buf, (uint32_t) &ret);
		free(absolutepath);
	} else {
		syscall(SYS_STAT, (uint32_t) dirname, (uint32_t) &buf, (uint32_t) &ret);
	}
	if (ret == 0 && S_ISDIR(buf.st_mode)) {
		DIR* dir = malloc(sizeof(DIR) + buf.st_blksize);
		dir->fd = open(dirname, O_RDONLY);
		dir->allocation = buf.st_blksize;
		dir->size = 0;
		dir->offset = 0;
		dir->filepos = 0;
		return dir;
	}
	return NULL;
}

struct dirent* readdir(DIR* dirp) {
	if (dirp->offset >= dirp->size) {
		int size = dirp->allocation;
		syscall(SYS_READDIR, (uint32_t) dirp->fd, (uint32_t) dirp->data, (uint32_t) &size);
		dirp->size = size;
	}
	
	if (dirp->size > 0) {
		struct dirent *d = (struct dirent*)(&(dirp->data) + dirp->filepos);
		dirp->offset += d->d_reclen;
		dirp->filepos += d->d_reclen;
		return d;
	}
	return NULL;
}

int closedir(DIR* dirp) {
	if (dirp) {
		free(dirp);
		return 0;
	}
	return 1;
}
