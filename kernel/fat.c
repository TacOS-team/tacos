#include <stdio.h>
#include <floppy.h>
#include <types.h>
#include "fat.h"

fat_BS_t Boot_Sector;
fat_dir_entry_t Root_Dir[8];

void mount_fat_fs () {
	int root_dir_sector;
	
	floppy_read_sector(0, 0, 0, (char*) &Boot_Sector);
	
	root_dir_sector = Boot_Sector.reserved_sector_count + Boot_Sector.table_count * Boot_Sector.table_size_16;
	floppy_read_sector(0, 0, root_dir_sector, (char*) &Root_Dir);

}

void list_dir_entries () {
	int i;
	char name[14];
	
	for (i=0;i<8;i++) {
		if ( Root_Dir[i].long_file_name[0] == 0x41 ){
			
			// Reconstitution du nom et conversion UTF16 vers UTF8
			name[0] =  Root_Dir[i].long_file_name[1];
			name[1] =  Root_Dir[i].long_file_name[3];
			name[2] =  Root_Dir[i].long_file_name[5];
			name[3] =  Root_Dir[i].long_file_name[7];
			name[4] =  Root_Dir[i].long_file_name[9];
			
			name[5] =  Root_Dir[i].long_file_name[14];
			name[6] =  Root_Dir[i].long_file_name[16];
			name[7] =  Root_Dir[i].long_file_name[18];
			name[8] =  Root_Dir[i].long_file_name[20];
			name[9] =  Root_Dir[i].long_file_name[22];
			name[10] =  Root_Dir[i].long_file_name[24];
			
			name[11] =  Root_Dir[i].long_file_name[28];
			name[12] =  Root_Dir[i].long_file_name[30];
			
			name[13] = "\0";
			
			printf("%s ", name);
		}
	}
	printf("\n");
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

