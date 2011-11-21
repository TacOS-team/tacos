/**
 * @file fat_internal.h
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

#ifndef _FAT_INTERNAL_H_
#define _FAT_INTERNAL_H_

#include <types.h>
#include <time.h>
#include <vfs.h>
#include <fs/devfs.h>

// FAT low level.

typedef struct _fat_BS {
// Boot Sector
	uint8_t   bootjmp[3];         //0x00
	uint8_t   oem_name[8];        //0x03
	uint16_t  bytes_per_sector;     //0x0b
	uint8_t   sectors_per_cluster;    //0x0d
	uint16_t  reserved_sector_count;    //0x0e
	uint8_t   table_count;        //0x10
	uint16_t  root_entry_count;     //0x11
	uint16_t  total_sectors_16;     //0x13
	uint8_t   media_type;         //0x15
	uint16_t  table_size_16;        //0x16
	uint16_t  sectors_per_track;      //0x18
	uint16_t  head_side_count;      //0x1a
	uint32_t  hidden_sector_count;    //0x1c
	uint32_t  total_sectors_32;     //0x20
} __attribute__ ((packed)) fat_BS_t; 

typedef struct _fat_extended_BIOS_16 {
// Extended BIOS Parameter Block (FAT12 / FAT16)
	uint8_t   bios_drive_num;       //0x24
	uint8_t   reserved;         //0x25
	uint8_t   ext_boot_signature;       //0x26
	uint32_t  volume_id;          //0x27
	uint8_t   volume_label[11];     //0x2b
	uint8_t   fat_type_label[8];      //0x36
	uint8_t   os_boot_code[448];      //0x3e
	uint16_t  boot_sector_sign;     //0x1fe
} __attribute__ ((packed)) fat_extended_BIOS_16_t;

typedef struct _fat_extended_BIOS_32 {
// Extended BIOS Parameter Block (FAT32)
	uint32_t  table_size_32;       //0x24
	uint16_t  fat_flags;           //0x28
	uint16_t  version;             //0x2a
	uint32_t  cluster_root_dir;    //0x2c
	uint16_t  sector_fs_info;      //0x30
	uint16_t  sector_bs_backup;    //0x32
	uint8_t   reserved[12];        //0x34
	uint8_t   bios_drive_num;      //0x40
	uint8_t   reserved2;           //0x41
	uint8_t   ext_boot_signature;  //0x42
	uint32_t  volume_id;           //0x43
	uint8_t   volume_label[11];    //0x47
	uint8_t   fat_type_label[8];   //0x52
	uint8_t   os_boot_code[420];   //0x5a
	uint16_t  boot_sector_sign;    //0x1fe
} __attribute__ ((packed)) fat_extended_BIOS_32_t;

typedef struct _fat_time {
	unsigned int seconds2 : 5;
	unsigned int minutes : 6;
	unsigned int hours : 5;
} __attribute__ ((packed)) fat_time_t;

typedef struct _fat_date {
	unsigned int day : 5;
	unsigned int month : 4;
	unsigned int year : 7;
} __attribute__ ((packed)) fat_date_t;

typedef struct _fat_dir_entry {
	char      utf8_short_name[8];
	char      file_extension[3];
	uint8_t   file_attributes;
	uint8_t   reserved;
	uint8_t   create_time_ms;
	fat_time_t  create_time;
	fat_date_t  create_date;
	fat_date_t  last_access_date;
	uint16_t  ea_index;
	fat_time_t  last_modif_time;
	fat_date_t  last_modif_date;
	uint16_t  cluster_pointer;
	uint32_t  file_size;

}__attribute__((packed)) fat_dir_entry_t;

typedef struct {
	uint8_t   seq_number;
	uint8_t   filename1[10];
	uint8_t   attributes;
	uint8_t   reserved; // always 0x0
	uint8_t   checksum;
	uint8_t   filename2[12];
	uint16_t  cluster_pointer; // always 0x000
	uint8_t   filename3[4];
}__attribute__((packed)) lfn_entry_t;


// FAT higher level.
typedef struct _directory_entry {
	char name[256];
	uint8_t attributes;
	uint32_t size;
	time_t access_time;
	time_t modification_time;
	time_t creation_time;
	uint32_t cluster;
	struct _directory_entry *next;
} directory_entry_t;

typedef struct _directory {
	directory_entry_t *entries; 
	int total_entries;
	char name[256];
	uint32_t cluster;
} directory_t;

typedef enum {
	FAT12,
	FAT16,
	FAT32
} fat_t;

// Structure qui contient tout ce qui est utilisé par le driver FAT.
typedef struct _fat_info {
	fat_BS_t BS;
	fat_extended_BIOS_16_t *ext_BIOS_16;
	fat_extended_BIOS_32_t *ext_BIOS_32;
	unsigned int *addr_fat;
	unsigned int addr_root_dir;
	unsigned int addr_data;
	unsigned int *file_alloc_table;
	unsigned int total_data_clusters;
	unsigned int table_size;
	fat_t fat_type;
	unsigned int bytes_per_cluster;
	blkdev_read_t read_data;
	blkdev_write_t write_data;
} fat_info_t;


typedef struct _fat_fs_instance_t {
	fs_instance_t super;
	fat_info_t fat_info;
} fat_fs_instance_t;

void read_fat(fat_fs_instance_t *instance);

#endif
