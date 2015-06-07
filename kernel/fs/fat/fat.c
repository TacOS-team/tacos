/**
 * @file fat.c
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
 * @brief Driver FAT.
 */

#include <fs/fat.h>
#include <kmalloc.h>
#include <klog.h>
#include <klibc/string.h>

#include "fat_internal.h"

static file_system_t fat_fs = {.name="FAT", .unique_inode=0, .mount=mount_FAT, .umount=umount_FAT};

void fat_init() {
	vfs_register_fs(&fat_fs);
}

static inline dentry_t * init_rootfatfs(fat_fs_instance_t *instance) {
	fat_direntry_t *root_fatfs = kmalloc(sizeof(fat_direntry_t));
	
	root_fatfs->fat_directory = open_root_dir(instance);
	root_fatfs->fat_entry = NULL;
	root_fatfs->super.d_pdentry = NULL;
	root_fatfs->super.d_name = "";
	root_fatfs->super.d_inode = kmalloc(sizeof(inode_t));
	memset(root_fatfs->super.d_inode, 0, sizeof(inode_t));
	root_fatfs->super.d_inode->i_count = 0;
	root_fatfs->super.d_inode->i_instance = (fs_instance_t*)instance;
	root_fatfs->super.d_inode->i_fs_specific = NULL;
	root_fatfs->super.d_inode->i_mode = 00755 | S_IFDIR;
	root_fatfs->super.d_inode->i_fops = &fatfs_fops;

	return (dentry_t*)root_fatfs;
}

/*
 * Init the FAT driver for a specific devide.
 */
fs_instance_t* mount_FAT(open_file_descriptor* ofd) {
	klog("mounting FAT");

	fat_fs_instance_t *instance = kmalloc(sizeof(fat_fs_instance_t));
	instance->super.fs = &fat_fs;
	instance->read_data = ((blkdev_interfaces*)(ofd->i_fs_specific))->read;
	instance->write_data = ((blkdev_interfaces*)(ofd->i_fs_specific))->write;
	instance->super.device = ofd;
	uint8_t sector[512];
	instance->read_data(instance->super.device, sector, 512, 0);
	memcpy(&(instance->fat_info.BS), sector, sizeof(fat_BS_t));

	if (instance->fat_info.BS.table_size_16 == 0) { // Si 0 alors on considère qu'on est en FAT32.
		instance->fat_info.ext_BIOS_16 = NULL;
		instance->fat_info.ext_BIOS_32 = kmalloc(sizeof(fat_extended_BIOS_32_t));

		memcpy(instance->fat_info.ext_BIOS_32, sector + sizeof(fat_BS_t), sizeof(fat_extended_BIOS_32_t));
		
		instance->fat_info.table_size = instance->fat_info.ext_BIOS_32->table_size_32;
	} else {
		instance->fat_info.ext_BIOS_32 = NULL;
		instance->fat_info.ext_BIOS_16 = kmalloc(sizeof(fat_extended_BIOS_16_t));

		memcpy(instance->fat_info.ext_BIOS_16, sector + sizeof(fat_BS_t), sizeof(fat_extended_BIOS_16_t));

		instance->fat_info.table_size = instance->fat_info.BS.table_size_16;
	}

	instance->fat_info.bytes_per_cluster = instance->fat_info.BS.bytes_per_sector * instance->fat_info.BS.sectors_per_cluster;

	kdebug("table size : %d", instance->fat_info.table_size);
	kdebug("%d bytes per logical sector", instance->fat_info.BS.bytes_per_sector);
	kdebug("%d bytes per clusters", instance->fat_info.BS.bytes_per_sector * instance->fat_info.BS.sectors_per_cluster);
	instance->fat_info.addr_fat = (unsigned int*) kmalloc(sizeof(unsigned int) * instance->fat_info.BS.table_count);
	
	int i;
	for (i = 0; i < instance->fat_info.BS.table_count; i++) {
		instance->fat_info.addr_fat[i] = (instance->fat_info.BS.reserved_sector_count + i * instance->fat_info.table_size) * instance->fat_info.BS.bytes_per_sector;
	}
	instance->fat_info.addr_root_dir = (instance->fat_info.BS.reserved_sector_count + instance->fat_info.BS.table_count * instance->fat_info.table_size) * instance->fat_info.BS.bytes_per_sector;
	instance->fat_info.addr_data = instance->fat_info.addr_root_dir + (instance->fat_info.BS.root_entry_count * sizeof(fat_dir_entry_t));
	if (instance->fat_info.BS.total_sectors_16 > 0)
		instance->fat_info.total_data_clusters = instance->fat_info.BS.total_sectors_16 / instance->fat_info.BS.sectors_per_cluster - instance->fat_info.addr_data / (instance->fat_info.BS.bytes_per_sector * instance->fat_info.BS.sectors_per_cluster);
	else
		instance->fat_info.total_data_clusters = instance->fat_info.BS.total_sectors_32 / instance->fat_info.BS.sectors_per_cluster - instance->fat_info.addr_data / (instance->fat_info.BS.bytes_per_sector * instance->fat_info.BS.sectors_per_cluster);

	if (instance->fat_info.total_data_clusters < 4086) {
		instance->fat_info.fat_type = FAT12;
		kdebug("FAT Type : FAT12");
	} else if (instance->fat_info.total_data_clusters < 65526) {
		instance->fat_info.fat_type = FAT16;
		kdebug("FAT Type : FAT16");
	} else {
		instance->fat_info.fat_type = FAT32;
		kdebug("FAT Type : FAT32");
	}

	kdebug("First FAT starts at byte %u (sector %u)", instance->fat_info.addr_fat[0], instance->fat_info.addr_fat[0] / instance->fat_info.BS.bytes_per_sector);
	kdebug("Root directory starts at byte %u (sector %u)", instance->fat_info.addr_root_dir, instance->fat_info.addr_root_dir / instance->fat_info.BS.bytes_per_sector);
	kdebug("Data area starts at byte %u (sector %u)", instance->fat_info.addr_data, instance->fat_info.addr_data / instance->fat_info.BS.bytes_per_sector);
	kdebug("Total clusters : %d", instance->fat_info.total_data_clusters);


	instance->fat_info.file_alloc_table = (unsigned int*) kmalloc(sizeof(unsigned int) * instance->fat_info.total_data_clusters);

	read_fat(instance);

//	instance->super.mknod = fat_mknod; //XXX
//	instance->super.mkdir = fat_mkdir; //XXX
//	instance->super.rename = fat_rename; //XXX
//	instance->super.setattr = fat_setattr; //XXX
//	instance->super.unlink = fat_unlink; //XXX
//	instance->super.rmdir = fat_rmdir; //XXX
//	instance->super.truncate = fat_truncate; //XXX
	
	instance->root = init_rootfatfs(instance);
	instance->super.lookup = fat_lookup;
	instance->super.getroot = fat_getroot;

	return (fs_instance_t*)instance;
}

void umount_FAT(fs_instance_t *instance) {
	kfree(instance);
}

