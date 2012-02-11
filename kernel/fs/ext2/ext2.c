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

static int ceil(float n) {
  return (n == (int)n) ? n : (int)(n+(n>=0));
} 

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

static void read_group_desc_table(ext2_fs_instance_t *instance) {
	int b = instance->superblock.s_first_data_block + 1;
	instance->n_groups = ceil((float)instance->superblock.s_blocks_count / (float)instance->superblock.s_blocks_per_group);
	instance->group_desc_table = kmalloc(sizeof(struct ext2_group_desc) * instance->n_groups);

	instance->read_data(instance->super.device, instance->group_desc_table, sizeof(struct ext2_group_desc) * instance->n_groups, b * (1024 << instance->superblock.s_log_block_size));
}

static void show_info(ext2_fs_instance_t *instance) {
	klog("Nombre d'inodes : %d", instance->superblock.s_inodes_count);
	klog("Taille d'un inode : %d", instance->superblock.s_inode_size);
	klog("Blocks libres : %d", instance->superblock.s_free_blocks_count);
	klog("Inodes libres : %d", instance->superblock.s_free_inodes_count);
	klog("Taille block : %d", 1024 << instance->superblock.s_log_block_size);
	klog("Blocks par groupe : %d", instance->superblock.s_blocks_per_group);
}

/*
 * Init the EXT2 driver for a specific devide.
 */
fs_instance_t* mount_EXT2(open_file_descriptor* ofd) {
	klog("mounting EXT2");
	ext2_fs_instance_t *instance = kmalloc(sizeof(ext2_fs_instance_t));

	instance->read_data = ((blkdev_interfaces*)(ofd->extra_data))->read;
	instance->write_data = ((blkdev_interfaces*)(ofd->extra_data))->write;

	instance->super.open = ext2_open;
	instance->super.mknod = ext2_mknod;
	instance->super.mkdir = ext2_mkdir;
	instance->super.stat = ext2_stat;
	instance->super.unlink = ext2_unlink;
	instance->super.truncate = ext2_truncate;

  instance->read_data(instance->super.device, &(instance->superblock), sizeof(struct ext2_super_block), 1024);
  read_group_desc_table(instance);
  show_info(instance);

	return (fs_instance_t*)instance;
}

void umount_EXT2(fs_instance_t *instance) {
	kfree(instance);
}

