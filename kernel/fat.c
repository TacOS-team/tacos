#include <stdio.h>
#include <floppy.h>
#include <types.h>
#include <string.h>
#include "fat.h"

// ceci sera calculé avec les donnée du Boot secteur qd on alloura 
// le tableau pour charger la file_alloc_table au malloc
// en attendant il est defini "en dur" donc on lui file comme taille cette valuer pres calcule
#define TOTAL_CLUSTERS 3072 //(9fat*512otect*8bits)/12bits   pour FAT12


fat_BS_t boot_sector;
fat_dir_entry_t root_dir[224];
fat_dir_entry_t sub_dir[224];
path_t path;
fat_info_t fat_info;
uint8_t buffer[512*9];

cluster_t file_alloc_table[TOTAL_CLUSTERS];


void sleep () {
	int c=0;
	while (c<1000000) c++;
}

addr_CHS_t get_CHS_from_LBA (addr_LBA_t sector_LBA) {
	addr_CHS_t ret;
	ret.Cylinder = sector_LBA / (fat_info.sectors_per_track*fat_info.head_side_count);
	ret.Head = (sector_LBA % (fat_info.sectors_per_track*fat_info.head_side_count)) / (fat_info.sectors_per_track);
	ret.Sector = (sector_LBA % (fat_info.sectors_per_track*fat_info.head_side_count)) % (fat_info.sectors_per_track);
	return ret;
}

addr_CHS_t get_CHS_from_cluster (cluster_t cluster) {
	addr_CHS_t ret;
	ret = get_CHS_from_LBA(cluster - 2 + fat_info.data_area_LBA);
	return ret;
}

void read_fat (cluster_t *fat) {
	
	addr_CHS_t sector_addr;
	int i,c;
	int p=0;
	uint32_t tmp = 0;
	char * p_buffer = (char*) buffer;
	
	for (i=0;i<fat_info.fat_size;i++) {
		sector_addr=get_CHS_from_LBA(fat_info.fat_LBA+i);
		floppy_read_sector(sector_addr.Cylinder, sector_addr.Head, sector_addr.Sector, p_buffer);
		p_buffer+=fat_info.bytes_per_sector;
		
	}
	
	// decodage FAT12
	for (i=0;i<TOTAL_CLUSTERS;i+=2) {
		
		 // on interprete les 3 octets comme un little endian (24bits) number
		tmp = buffer[p] + ((buffer[p+1]<<8)&0xFF00) + ((buffer[p+2]<<16)&0xFF0000); 
		
		// on extrait les 2 clusters de 12bits
		fat[i] = (tmp&0xFFF); // 12 least significant bits
		fat[i+1] = (((tmp&0xFFF000)>>12)&0xFFF); // 12 most significant bits
		
		p+=3;
	}
	
}

void read_root_dir (fat_dir_entry_t * rdir) {
	addr_CHS_t sector_addr;
	char * p_root_dir = (char*) rdir;
	int i,c;
	for (i=0;i<fat_info.cluster1_sector_count;i++) {
		sector_addr = get_CHS_from_LBA(fat_info.root_dir_LBA+i);
		floppy_read_sector(sector_addr.Cylinder, sector_addr.Head, sector_addr.Sector, p_root_dir);
		p_root_dir+=fat_info.bytes_per_sector;
	}
}

uint8_t * read_cluster (uint8_t * buf, cluster_t cluster) {
	addr_CHS_t sector_addr;
	if ( (cluster!=0) && (cluster!=1) ) {
		sector_addr = get_CHS_from_cluster(cluster);
		floppy_read_sector(sector_addr.Cylinder, sector_addr.Head, sector_addr.Sector, (char*) buf);
		//printf("\n%d %d %d\n",sector_addr.Cylinder, sector_addr.Head, sector_addr.Sector);
	}
	else {
		printf("ERROR: tentative de lecture d un secteur reserve\n");
	}
	return buf;
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

void open_root_dir () {
	int i;
	int itermax = fat_info.root_entry_count;
	char str[14];
	char name[14] = "root";
	read_root_dir(root_dir);
	path.current = 0;
	strcpy(path.name[path.current],name);
	path.content[path.current].total_entries = 0;
	for(i=0;i<itermax;i++) {
		if ( root_dir[i].long_file_name[0] == 0x41 ){
			decode_long_file_name (str, root_dir[i].long_file_name);
			strcpy(path.content[path.current].entry_name[path.content[path.current].total_entries],str);
			path.content[path.current].entry_cluster[path.content[path.current].total_entries] = root_dir[i].cluster_pointer;
			path.content[path.current].total_entries++;
		}
	}
}

void open_next_dir(char * name) {
	cluster_t next;
	char str[14];
	int i,k;
	for(i=0;i<path.content[path.current].total_entries;i++) {
		if(strcmp(path.content[path.current].entry_name[i],name)==0) {
			next = path.content[path.current].entry_cluster[i];
			break;
		}
	}
	read_cluster (sub_dir, next);
	path.current++;
	strcpy(path.name[path.current],name);
	path.content[path.current].total_entries = 0;
	for(i=0;i<8;i++) {
		if ( sub_dir[i].long_file_name[0] == 0x41 ){
			decode_long_file_name (str, sub_dir[i].long_file_name);
			strcpy(path.content[path.current].entry_name[path.content[path.current].total_entries],str);
			path.content[path.current].entry_cluster[path.content[path.current].total_entries] = sub_dir[i].cluster_pointer;
			path.content[path.current].total_entries++;
		}
	}
}
void open_prev_dir() {
	path.current--;
}

void mount_FAT12 () {
	
	floppy_read_sector(0, 0, 0, (char*) &boot_sector);
	
	fat_info.head_side_count = boot_sector.head_side_count;
	fat_info.sectors_per_track = boot_sector.sectors_per_track;
	fat_info.total_sectors = boot_sector.total_sectors_16;
	fat_info.bytes_per_sector = boot_sector.bytes_per_sector;
	fat_info.sectors_per_cluster = boot_sector.sectors_per_cluster;
	fat_info.reserved_sector_count = boot_sector.reserved_sector_count;
	fat_info.hidden_sector_count = boot_sector.hidden_sector_count;
	fat_info.fat_size = boot_sector.table_size_16;
	fat_info.fat_count = boot_sector.table_count;
	fat_info.root_entry_count = boot_sector.root_entry_count;
	
	fat_info.total_clusters = (fat_info.fat_size * fat_info.bytes_per_sector * 8) / 12; // Pour le FAT12
	fat_info.cluster0_sector_count = fat_info.fat_size * fat_info.fat_count;
	fat_info.cluster1_sector_count = (fat_info.root_entry_count * SIZEOF_DIR_ENTRY) / (fat_info.bytes_per_sector);
	fat_info.fat_LBA = fat_info.reserved_sector_count;
	fat_info.root_dir_LBA = fat_info.fat_LBA + fat_info.cluster0_sector_count;
	fat_info.data_area_LBA = fat_info.root_dir_LBA + fat_info.cluster1_sector_count;
	
	//path.current = 0;
	//path.list[path.current] = fat_info.root_dir_cluster;
	//strcpy(path.name[path.current],"root");
	
	read_fat(file_alloc_table);
	//read_root_dir(root_dir);
	open_root_dir();
	
}



/*
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

*/
void change_dir (char * target_name) {
	//printf("\nstill working, come back later please..\n");
	int i;
	int itermax = path.content[path.current].total_entries;
	for(i=0;i<itermax;i++) {
		if ( strcmp(path.content[path.current].entry_name[i],target_name) == 0){
			open_next_dir(path.content[path.current].entry_name[i]);
			break;
		}
	}
/*
	int i;
	char name[14];
	
	if (strcmp(target_name, "..") == 0) {
		if (path.current == 0 ) {
			printf("cd: impossible d'acceder a repertoire .. depuis root/\n");
		}
		else {
			path.current = path.current -1;
			open_working_dir ();
		}
	}
	else {
		for (i=0;i<8;i++) {
			if (strcmp(target_name, decode_long_file_name (name, current_dir[i].long_file_name)) == 0) {
				path.current = path.current + 1;
				path.list[path.current] = current_dir[i].cluster_pointer;
				strcpy(path.name[path.current],name);
				open_working_dir ();
			}
		}
	}
	*/
}


void list_segments () {
	int i;
	char name[14];
	for (i=0;i<path.content[path.current].total_entries;i++) {
		
			printf("%s(%d) ",path.content[path.current].entry_name[i], path.content[path.current].entry_cluster[i]);
		
	}
}

void print_working_dir () {
	//int i;
	
	//for (i=0;i<=path.current;i++)
	//	printf("%s/",path.name[i]);
	//printf("\nstill working, come back later please..\n");
}

void print_Boot_Sector () {
	boot_sector.oem_name[7] = '\0';
	boot_sector.volume_label[10] = '\0';
	boot_sector.fat_type_label[8] = '\0';
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
 Hidden sector count\
 FAT type: %s\n\
 Boot signature: 0x%x\n",
	boot_sector.oem_name,
	boot_sector.bytes_per_sector,
	boot_sector.sectors_per_cluster,
	boot_sector.reserved_sector_count,
	boot_sector.table_count,
	boot_sector.root_entry_count,
	boot_sector.total_sectors_16,
	boot_sector.media_type,
	boot_sector.table_size_16,
	boot_sector.volume_label,
	boot_sector.fat_type_label,
	boot_sector.hidden_sector_count,
	boot_sector.boot_signature);
	
	printf("\
	fat_info.head_side_count %d\n\
	fat_info.sectors_per_track %d\n\
	fat_info.total_sectors %d\n\
	fat_info.bytes_per_sector %d\n\
	fat_info.sectors_per_cluster %d\n\
	fat_info.reserved_sector_count %d\n\
	fat_info.hidden_sector_count %d\n\
	fat_info.fat_size %d\n\
	fat_info.fat_count %d\n\
	fat_info.root_entry_count %d\n\
	fat_info.total_clusters %d\n\
	fat_info.cluster0_sector_count %d\n\
	fat_info.cluster1_sector_count %d\n\
	fat_info.fat_LBA %d\n\
	fat_info.root_dir_LBA %d\n\
	fat_info.data_area_LBA %d\n",
	fat_info.head_side_count,
	fat_info.sectors_per_track, 
	fat_info.total_sectors, 
	fat_info.bytes_per_sector, 
	fat_info.sectors_per_cluster, 
	fat_info.reserved_sector_count, 
	fat_info.hidden_sector_count,
	fat_info.fat_size, 
	fat_info.fat_count, 
	fat_info.root_entry_count, 
	fat_info.total_clusters, 
	fat_info.cluster0_sector_count, 
	fat_info.cluster1_sector_count, 
	fat_info.fat_LBA,
	fat_info.root_dir_LBA, 
	fat_info.data_area_LBA); 
}


void print_path () {
		int i=206;
		int c=1;
		while (file_alloc_table[i]!= 0xFFF) {
			printf("%d ",file_alloc_table[i]);
			c++;
			i=file_alloc_table[i];
		}
		printf("%d\n",file_alloc_table[i]);
		printf("count sector: %d\n",c);
		
		printf("\nstill working, come back later please..\n");
}


