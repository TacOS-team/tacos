/**
 * @file fs/ext2.h
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
 * Description de ce que fait le fichier
 */

#ifndef _EXT2_H_
#define _EXT2_H_

#include <vfs.h>

void ext2_init();
fs_instance_t* mount_EXT2(open_file_descriptor* ofd);
void umount_EXT2(fs_instance_t *instance);

open_file_descriptor * ext2_open(fs_instance_t *instance, const char * path, uint32_t flags);
size_t ext2_read(open_file_descriptor * ofd, void * buf, size_t size);
int ext2_readdir(fs_instance_t *instance, const char * path, int iter, char * filename);
int ext2_stat(fs_instance_t *instance, const char *path, struct stat *stbuf);
int ext2_mkdir(fs_instance_t *instance, const char * path, mode_t mode);
int ext2_unlink(fs_instance_t *instance, const char * path);

#endif
