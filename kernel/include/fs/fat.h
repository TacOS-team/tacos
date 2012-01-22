/**
 * @file fs/fat.h
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

#ifndef _KFAT_H_
#define _KFAT_H_

#include <types.h>
#include <fd_types.h>
#include <sys/stat.h>
#include <vfs.h>

void fat_init();
fs_instance_t* mount_FAT(open_file_descriptor* ofd);
void umount_FAT(fs_instance_t *instance);

int fat_readdir(fs_instance_t *instance, const char * path, int iter, char * filename);
int fat_opendir(fs_instance_t *instance, const char * path);

open_file_descriptor * fat_open_file(fs_instance_t *instance, const char * path, uint32_t flags);
int fat_mkdir(fs_instance_t *instance, const char * path, mode_t mode);
int fat_stat(fs_instance_t *instance, const char *path, struct stat *stbuf);
int fat_unlink(fs_instance_t *instance, const char * path);

size_t fat_read_file (open_file_descriptor * ofd, void * buf, size_t count);
size_t fat_write_file (open_file_descriptor * ofd, const void * buf, size_t nb_octet);
int fat_seek_file (open_file_descriptor * ofd, long offset, int whence);
int fat_close(open_file_descriptor *ofd);

#endif
