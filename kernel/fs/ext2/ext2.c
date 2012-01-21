/**
 * @file ext2.c
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
 * EXT2 File System.
 */

#include <fs/ext2.h>
#include <klog.h>
#include <kmalloc.h>

#include "ext2_internal.h"

/*
 * Register this FS.
 */
void ext2_init() {
	file_system_t *fs = kmalloc(sizeof(file_system_t));
	fs->name = "EXT2";
	fs->mount = mount_EXT2;
	fs->umount = umount_EXT2;
	vfs_register_fs(fs);
}

/*
 * Init the FAT driver for a specific devide.
 */
fs_instance_t* mount_EXT2(open_file_descriptor* ofd) {
	klog("mounting EXT2");
	ext2_fs_instance_t *instance = kmalloc(sizeof(ext2_fs_instance_t));

	instance->read_data = ((blkdev_interfaces*)(ofd->extra_data))->read;
	instance->write_data = ((blkdev_interfaces*)(ofd->extra_data))->write;

	return (fs_instance_t*)instance;
}

void umount_EXT2(fs_instance_t *instance) {
	kfree(instance);
}

