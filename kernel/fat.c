#include <stdio.h>
#include <floppy.h>
#include <types.h>
#include <string.h>
#include "fat.h"


fat_BS_t Boot_Sector;
fat_dir_entry_t Root_Dir[8];
fat_dir_entry_t tmp_Dir[8];

virtual_FS_dir_t virtual_FS_root_dir;

virtual_FS_dir_t * current_vFS__dir = &virtual_FS_root_dir;


addr_CHS_t convert_LBA_to_CHS (int sector_LBA) {
	addr_CHS_t ret;
	
	ret.Cylinder = sector_LBA / (NB_SECTORS*NB_HEADS);
	ret.Head = (sector_LBA % (NB_SECTORS*NB_HEADS)) / NB_SECTORS;
	ret.Sector = (sector_LBA % (NB_SECTORS*NB_HEADS)) % NB_SECTORS;
	
	return ret;
}

void mount_fat_fs () {
	int root_dir_sector;
	char * name = "root";
	floppy_read_sector(0, 0, 0, (char*) &Boot_Sector);
	
	root_dir_sector = Boot_Sector.reserved_sector_count + Boot_Sector.table_count * Boot_Sector.table_size_16;
	floppy_read_sector(0, 0, root_dir_sector, (char*) Root_Dir);

	strcpy(virtual_FS_root_dir.name,name);
	update_virtual_FS (&virtual_FS_root_dir,Root_Dir);
}


void change_dir (char * target_name) {
	
	int i;
	int sector_num;
	addr_CHS_t sector_addr;
	int offset;
	int reserved;
	
	for (i=0;i<current_vFS__dir->entries_count;i++) {
	
		if (strcmp(target_name, current_vFS__dir->sub_directories[i]->name) == 0) {
			reserved = Boot_Sector.reserved_sector_count + Boot_Sector.table_count * Boot_Sector.table_size_16;
			offset = (Boot_Sector.root_entry_count * 32) / (Boot_Sector.bytes_per_sector) ;
			sector_num =  reserved + offset + current_vFS__dir->sub_directories[i]->cluster_pointer - 2;
			sector_addr=convert_LBA_to_CHS(sector_num);
			//printf("offset:%d reserved:%d sector:%d cluster:%d",offset,reserved,sector_num,current_vFS__dir->sub_directories[i]->cluster_pointer);
			//floppy_read_sector(0, 1, 15, (char*) tmp_Dir);
			floppy_read_sector(sector_addr.Cylinder, sector_addr.Head, sector_addr.Sector, (char*) tmp_Dir);
			update_virtual_FS (current_vFS__dir->sub_directories[i],tmp_Dir);
			current_vFS__dir = current_vFS__dir->sub_directories[i];
		}
	}
}

void update_virtual_FS (virtual_FS_dir_t * vFS_dir, fat_dir_entry_t * fat_dir) {
	int i;
	int count = 0;
	char name[14];
	name[13] = "\0";
	
	for (i=0;i<8;i++) {
		if ( fat_dir[i].long_file_name[0] == 0x41 ){
			
			vFS_dir->sub_directories[count] = kmalloc(sizeof(virtual_FS_dir_t));
			
			// Reconstitution du nom et conversion UTF16 vers UTF8 
			// dont le mecanisme a ete obtenu par une methode comunement appellee "reverse engineering"
			name[0] =  fat_dir[i].long_file_name[1];
			name[1] =  fat_dir[i].long_file_name[3];
			name[2] =  fat_dir[i].long_file_name[5];
			name[3] =  fat_dir[i].long_file_name[7];
			name[4] =  fat_dir[i].long_file_name[9];
			
			name[5] =  fat_dir[i].long_file_name[14];
			name[6] =  fat_dir[i].long_file_name[16];
			name[7] =  fat_dir[i].long_file_name[18];
			name[8] =  fat_dir[i].long_file_name[20];
			name[9] =  fat_dir[i].long_file_name[22];
			name[10] =  fat_dir[i].long_file_name[24];
			
			name[11] =  fat_dir[i].long_file_name[28];
			name[12] =  fat_dir[i].long_file_name[30];
			
			strcpy(vFS_dir->sub_directories[count]->name,name);
			
			vFS_dir->sub_directories[count]->cluster_pointer = fat_dir->cluster_pointer;
			vFS_dir->sub_directories[count]->root_dir = current_vFS__dir;
			
			vFS_dir->entries_count = count + 1;
			count++;
		}
	}
	
}

void list_dir_entries () {
	
	int i;
	for (i=0;i<current_vFS__dir->entries_count;i++) {
		printf("%s ",current_vFS__dir->sub_directories[i]->name);
	}
	printf("\n");
}

void present_dir () {
	printf("%s\n",current_vFS__dir->name);
}

void read_fat_BS () {
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

