#ifndef _FAT_H_
#define _FAT_H_

#include <types.h>

#define NB_CYLINDERS 80
#define NB_HEADS 2
#define NB_SECTORS 18 // NB Sector by Track

#define SIZEOF_DIR_ENTRY 32 //octet


typedef uint32_t cluster_t;
typedef uint32_t addr_LBA_t;
typedef struct addr_CHS { uint32_t Cylinder; uint32_t Head; uint32_t Sector; } addr_CHS_t;

typedef struct _directory {
	uint8_t entry_name[100][14];
	cluster_t entry_cluster[100];
	int total_entries;
} directory_t;

typedef struct _path {
	directory_t content[100];
	char name[100][14];
	int current;
} path_t;

typedef struct _fat_info {
	
	// Storage Information
	uint32_t	bytes_per_sector;		// Usually 512 bytes
	uint32_t	head_side_count;		// 2 for floppy
	uint32_t	sectors_per_track;		// 18 for floppy
	uint32_t	total_sectors;
	// File System Information
	uint32_t	sectors_per_cluster;
	uint32_t	reserved_sector_count;	// Usually 1 (Boot Sector)
	uint32_t	hidden_sector_count;
	uint32_t	fat_size;
	uint32_t	fat_count; 				// NB of File Allocation tables (usually 2)
	uint32_t	root_entry_count;		// NB max entries in Root Directory
	uint32_t	total_clusters;
	uint32_t	cluster0_sector_count;	// Cluster used by the File Allocation Tables 
	uint32_t	cluster1_sector_count;	// Cluster used by the Root directory
	// Computed LBAs
	addr_LBA_t	fat_LBA;					// LBA of First sector of cluster 0
	addr_LBA_t	root_dir_LBA;			// LBA of First sector of cluster 1
	addr_LBA_t	data_area_LBA;			// LBA of First sector of cluster 2 (first cluster of the data area)
	
} fat_info_t;

typedef struct _fat_BS {
											//Offset
	uint8_t		bootjmp[3];					//0x00
	uint8_t		oem_name[8];				//0x03
	uint16_t	bytes_per_sector;			//0x0b
	uint8_t		sectors_per_cluster;		//0x0d
	uint16_t	reserved_sector_count;		//0x0e
	uint8_t		table_count;				//0x10
	uint16_t	root_entry_count;			//0x11
	uint16_t	total_sectors_16;			//0x13
	uint8_t		media_type;					//0x15
	uint16_t	table_size_16;				//0x16
	uint16_t	sectors_per_track;			//0x18
	uint16_t	head_side_count;			//0x1a
	uint32_t	hidden_sector_count;		//0x1c
	uint32_t	total_sectors_32;			//0x20
	uint8_t		bios_drive_num;				//0x24
	uint8_t		reserved;					//0x25
	uint8_t		boot_signature;				//0x26
	uint32_t	volume_id;					//0x27
	uint8_t		volume_label[11];			//0x2b
	uint8_t		fat_type_label[8];			//0x36
	uint8_t		os_boot_code[448];			//0x3e
	uint16_t	bs_signature;				//0x1fe
	
}__attribute__((packed)) fat_BS_t;

typedef struct _fat_dir_entry {
	
	uint8_t		long_file_name[32];
	uint8_t		utf8_short_name[8];
	uint8_t		file_extension[3];
	uint8_t		file_attributes;
	uint8_t		reserved;
	uint8_t		create_time_ms;
	uint16_t	create_time;
	uint16_t	create_date;
	uint16_t	last_access_date;
	uint16_t	ea_index;
	uint16_t	last_modif_time;
	uint16_t	last_modif_date;
	uint16_t	cluster_pointer;
	uint32_t	file_size;
	
}__attribute__((packed)) fat_dir_entry_t;




void mount_FAT12 ();
//void open_working_dir ();

void print_Boot_Sector ();

void list_segments ();
void print_working_dir ();

void print_path ();

#endif
