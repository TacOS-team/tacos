#include <stdio.h>
#include <floppy.h>
#include <types.h>
#include <string.h>
#include "fat.h"


fat_BS_t Boot_Sector;
fat_dir_entry_t current_dir[112];
path_t path;
fat_info_t fat_info;

void print_path () {
	int i;
	for (i=0;i<=path.current;i++)
		printf(" %d ", path.list[i]);
}

addr_CHS_t get_CHS_from_LBA (addr_LBA_t sector_LBA) {
	addr_CHS_t ret;
	ret.Cylinder = sector_LBA / (NB_SECTORS*NB_HEADS);
	ret.Head = (sector_LBA % (NB_SECTORS*NB_HEADS)) / NB_SECTORS;
	ret.Sector = (sector_LBA % (NB_SECTORS*NB_HEADS)) % NB_SECTORS;
	return ret;
}

addr_CHS_t get_CHS_from_cluster (cluster_t cluster) {
	addr_CHS_t ret;
	ret = get_CHS_from_LBA(cluster - 2 + fat_info.data_segment_LBA);
	return ret;
}

char * decode_long_file_name (char * name, uint8_t * long_file_name ) {
	
			// Reconstitution du nom et conversion UTF16 vers UTF8 
			// dont le mecanisme a ete obtenu par une methode comunement appellee "reverse engineering"
			name[0]  =  long_file_name[1];
			name[1]  =  long_file_name[3];
			name[2]  =  long_file_name[5];
			name[3]  =  long_file_name[7];
			name[4]  =  long_file_name[9];
			
			name[5]  =  long_file_name[14];
			name[6]  =  long_file_name[16];
			name[7]  =  long_file_name[18];
			name[8]  =  long_file_name[20];
			name[9]  =  long_file_name[22];
			name[10] =  long_file_name[24];
			
			name[11] =  long_file_name[28];
			name[12] =  long_file_name[30];
			
			name[13] = "\0";

	return name;
}

void mount_FAT12 () {
	
	floppy_read_sector(0, 0, 0, (char*) &Boot_Sector);
	
	fat_info.root_dir_LBA = Boot_Sector.reserved_sector_count + Boot_Sector.table_count * Boot_Sector.table_size_16;
	fat_info.data_segment_LBA = fat_info.root_dir_LBA + (Boot_Sector.root_entry_count * 32) / (Boot_Sector.bytes_per_sector);
	fat_info.root_dir_cluster = 1;
	
	path.current = 0;
	path.list[path.current] = fat_info.root_dir_cluster;
	strcpy(path.working_dir,"root");
	
	open_working_dir ();
}

void open_working_dir () {
	addr_CHS_t sector_addr;
	if (path.list[path.current] == fat_info.root_dir_cluster) {
		sector_addr = get_CHS_from_LBA(fat_info.root_dir_LBA);
	}
	else {
		sector_addr = get_CHS_from_cluster(path.list[path.current]);
		//printf("CHS:%d %d %d\n",sector_addr.Cylinder, sector_addr.Head, sector_addr.Sector);
	}
	floppy_read_sector(sector_addr.Cylinder, sector_addr.Head, sector_addr.Sector, (char*) current_dir);
}


void change_dir (char * target_name) {
	
	int i;
	char name[14];
	
	if (strcmp(target_name, "..") == 0) {
		path.current = path.current -1;
		open_working_dir ();
	}
	else {
		for (i=0;i<8;i++) {
			if (strcmp(target_name, decode_long_file_name (name, current_dir[i].long_file_name)) == 0) {
				path.current = path.current + 1;
				path.list[path.current] = current_dir[i].cluster_pointer;
				strcpy(path.working_dir,name);
				open_working_dir ();
			}
		}
	}
	
}


void list_segments () {
	int i;
	char name[14];
	for (i=0;i<8;i++) {
		if ( current_dir[i].long_file_name[0] == 0x41 ){
			decode_long_file_name (name, current_dir[i].long_file_name);
			printf("%s ",name);
		}
	}
}

void print_working_dir () {
	printf("%s\n",path.working_dir);
}

void print_Boot_Sector () {
	Boot_Sector.oem_name[7] = '\0';
	Boot_Sector.volume_label[10] = '\0';
	Boot_Sector.fat_type_label[8] = '\0';
	printf("\
Partiton Boot Sector info :\n\
 formated by: %s\n\
 sector size: %d\n\
 cluster size: %d\n\
 reserved sectors: %d\n\
 nb tables: %d\n\
 nb root entry: %d\n\
 total sectors: %d\n\
 media type: %x\n\
 table size: %d\n\
 volume label: %s\n\
 FAT type: %s\n",
	Boot_Sector.oem_name,
	Boot_Sector.bytes_per_sector,
	Boot_Sector.sectors_per_cluster,
	Boot_Sector.reserved_sector_count,
	Boot_Sector.table_count,
	Boot_Sector.root_entry_count,
	Boot_Sector.total_sectors_16,
	Boot_Sector.media_type,
	Boot_Sector.table_size_16,
	Boot_Sector.volume_label,
	Boot_Sector.fat_type_label);
}

