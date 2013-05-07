/**
 * @file ext2.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012, 2013 - TacOS developers.
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

static file_system_t ext2_fs = {.name="EXT2", .unique_inode=1, .mount=mount_EXT2, .umount=umount_EXT2};

static int ceil(float n) {
  return (n == (int)n) ? n : (int)(n+(n>=0));
} 

/*
 * Register this FS.
 */
void ext2_init() {
	vfs_register_fs(&ext2_fs);
}

static void read_group_desc_table(ext2_fs_instance_t *instance) {
	int b = instance->superblock.s_first_data_block + 1;
	instance->n_groups = ceil((float)instance->superblock.s_blocks_count / (float)instance->superblock.s_blocks_per_group);
	instance->group_desc_table = kmalloc(sizeof(struct ext2_group_desc) * instance->n_groups);

	instance->read_data(instance->super.device, instance->group_desc_table, sizeof(struct ext2_group_desc) * instance->n_groups, b * (1024 << instance->superblock.s_log_block_size));

	// Init de la copie mémoire
	instance->group_desc_table_internal = kmalloc(sizeof(struct ext2_group_desc) * instance->n_groups);
	int i;
	for (i = 0; i < instance->n_groups; i++) {
		// Chargement inode bitmap.
		instance->group_desc_table_internal[i].inode_bitmap = kmalloc(instance->superblock.s_inodes_per_group / 8);
		int addr_bitmap = instance->group_desc_table[i].bg_inode_bitmap;
		instance->read_data(instance->super.device, instance->group_desc_table_internal[i].inode_bitmap, instance->superblock.s_inodes_per_group / 8, addr_bitmap * (1024 << instance->superblock.s_log_block_size));

		// Chargement inodes.
		instance->group_desc_table_internal[i].inodes = kmalloc(instance->superblock.s_inodes_per_group * sizeof(struct ext2_inode));
		int addr_table = instance->group_desc_table[i].bg_inode_table;
		instance->read_data(instance->super.device, instance->group_desc_table_internal[i].inodes, instance->superblock.s_inodes_per_group * sizeof(struct ext2_inode), addr_table * (1024 << instance->superblock.s_log_block_size));
	}
}

static void show_info(ext2_fs_instance_t *instance) {
	klog("Nombre d'inodes : %d", instance->superblock.s_inodes_count);
	klog("Taille d'un inode : %d", instance->superblock.s_inode_size);
	klog("Blocks libres : %d", instance->superblock.s_free_blocks_count);
	klog("Inodes libres : %d", instance->superblock.s_free_inodes_count);
	klog("Taille block : %d", 1024 << instance->superblock.s_log_block_size);
	klog("Blocks par groupe : %d", instance->superblock.s_blocks_per_group);
}

static inline dentry_t * init_rootext2fs(ext2_fs_instance_t *instance) {
	dentry_t *root_ext2fs = kmalloc(sizeof(dentry_t));
	
	root_ext2fs->d_name = "";
	root_ext2fs->d_inode = kmalloc(sizeof(inode_t));
	root_ext2fs->d_inode->i_ino = EXT2_ROOT_INO;
	root_ext2fs->d_inode->i_instance = (fs_instance_t*)instance;
	ext2_extra_data *ext = kmalloc(sizeof(ext2_extra_data));
	ext->inode = EXT2_ROOT_INO;
	ext->type = EXT2_FT_DIR;
  root_ext2fs->d_inode->i_fs_specific = ext;
	root_ext2fs->d_inode->i_mode = S_IFDIR | 00755;
	root_ext2fs->d_inode->i_fops = &ext2fs_fops;

	return root_ext2fs;
}

/*
 * Init the EXT2 driver for a specific devide.
 */
fs_instance_t* mount_EXT2(open_file_descriptor* ofd) {
	klog("mounting EXT2");
	ext2_fs_instance_t *instance = kmalloc(sizeof(ext2_fs_instance_t));

	instance->super.fs = &ext2_fs;
	instance->root = init_rootext2fs(instance);

	instance->read_data = ((blkdev_interfaces*)(ofd->extra_data))->read;
	instance->write_data = ((blkdev_interfaces*)(ofd->extra_data))->write;

	instance->super.mknod = ext2_mknod;
	instance->super.mkdir = ext2_mkdir;
	instance->super.rename = ext2_rename;
	instance->super.setattr = ext2_setattr;
	instance->super.unlink = ext2_unlink;
	instance->super.rmdir = ext2_rmdir;
	instance->super.truncate = ext2_truncate;
	instance->super.lookup = ext2_lookup;
	instance->super.getroot = ext2_getroot;

  instance->read_data(instance->super.device, &(instance->superblock), sizeof(struct ext2_super_block), 1024);
  read_group_desc_table(instance);
  show_info(instance);

	return (fs_instance_t*)instance;
}

void umount_EXT2(fs_instance_t *instance) {
	kfree(instance);
}

