/**
 * @file dirent.h
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
 * Définition de DIR et struct dirent.
 */

#ifndef _DIRENT_H_
#define _DIRENT_H_

#include <sys/types.h>
#include <sys/stat.h>

#define NAME_MAX 256

typedef struct _DIR {
	int iter;
	char *path;
} DIR;

struct dirent {
	char d_name[NAME_MAX];
};

int mkdir(const char *pathname, mode_t mode);
DIR* opendir(const char* dirname);
struct dirent* readdir(DIR* dirp);
int closedir(DIR* dirp);



#endif
