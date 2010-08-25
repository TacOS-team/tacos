#include <stdio.h>
#include <floppy.h>
#include <types.h>
#include <string.h>
#include <fat.h>
#include <fcntl.h>
#include <debug.h>

// ceci sera calculé avec les donnée du Boot secteur qd on alloura 
// le tableau pour charger la file_alloc_table au malloc
// en attendant il est defini "en dur" donc on lui file comme taille cette valuer pres calcule
#define TOTAL_CLUSTERS 3072 //(9fat*512otect*8bits)/12bits   pour FAT12

static path_t path; //TODO: Dégager vers le shell !
static fat_info_t fat_info;
static cluster_t file_alloc_table[TOTAL_CLUSTERS];

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
	uint8_t buffer[512*9];
	addr_CHS_t sector_addr;
	uint32_t i;
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
	uint32_t i;
	for (i=0;i<fat_info.cluster1_sector_count;i++) {
		sector_addr = get_CHS_from_LBA(fat_info.root_dir_LBA+i);
		floppy_read_sector(sector_addr.Cylinder, sector_addr.Head, sector_addr.Sector, p_root_dir);
		p_root_dir+=fat_info.bytes_per_sector;
	}
}

int read_cluster (char * buf, cluster_t cluster) {
	addr_CHS_t sector_addr;
	if ( (cluster!=0) && (cluster!=1) ) {
		sector_addr = get_CHS_from_cluster(cluster);
		floppy_read_sector(sector_addr.Cylinder, sector_addr.Head, sector_addr.Sector, buf);
		return 0;
	}
	else {
		kprintf("ERROR: tentative de lecture d un secteur reserve\n");
		return 1;
	}
}

int write_cluster (uint8_t * buf, cluster_t cluster) {
	addr_CHS_t sector_addr;
	if ( (cluster!=0) && (cluster!=1) ) {
		sector_addr = get_CHS_from_cluster(cluster);
		floppy_write_sector(sector_addr.Cylinder, sector_addr.Head, sector_addr.Sector, (char*) buf);
		return 0;
	}
	else {
		kprintf("ERROR: tentative d'écriture d un secteur reserve\n");
		return 1;
	}
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
			
			name[13] = '\0';

	return name;
}
static fat_dir_entry_t root_dir[224];

void open_root_dir (directory_t * dir) {
	int i;
	int itermax = 8;//fat_info.root_entry_count/2;
	char str[14];
	char name[14];// = "fd0:";
	read_root_dir(root_dir);
	strcpy(dir->name,name);
	dir->total_entries = 0;
	for(i=0;i<itermax;i++) {
		if ( root_dir[i].long_file_name[0] == 0x41 ){
			decode_long_file_name (str, root_dir[i].long_file_name);
			strcpy(dir->entry_name[dir->total_entries],str);
			dir->entry_cluster[dir->total_entries] = root_dir[i].cluster_pointer;
			dir->entry_size[dir->total_entries] = root_dir[i].file_size;
			dir->entry_attributes[dir->total_entries] = root_dir[i].file_attributes;
			dir->total_entries++;
		}
	}
}

static fat_dir_entry_t sub_dir[8];

int open_next_dir(directory_t * prev_dir,directory_t * next_dir, char * name) {
	
	cluster_t next = 0;
	char str[14];
	int i;
	
	for(i=0;i<(prev_dir->total_entries);i++) {
		if(strcmp(prev_dir->entry_name[i],name)==0) {
				if ( (prev_dir->entry_attributes[i] & 0x10) == 0x10) {  //c'est bien un repertoire
					next = prev_dir->entry_cluster[i];
					break;
				}
				else {
						return 2;
				}
		}
	}
	
	if (next!=0) {
		read_cluster ((char*)sub_dir, next);
		strcpy(next_dir->name,name);
		next_dir->cluster = next;
		next_dir->total_entries = 0;
		for(i=0;i<8;i++) {
			if ( sub_dir[i].long_file_name[0] == 0x41 ){
				decode_long_file_name (str, sub_dir[i].long_file_name);
				strcpy(next_dir->entry_name[next_dir->total_entries],str);
				next_dir->entry_cluster[next_dir->total_entries] = sub_dir[i].cluster_pointer;
				next_dir->entry_size[next_dir->total_entries] = sub_dir[i].file_size;
				next_dir->entry_attributes[next_dir->total_entries] = sub_dir[i].file_attributes;
				next_dir->total_entries++;
			}
		}
		return 0;
	}
	else
		return 1;
}


uint8_t file_buffer[512];

int get_dirname_from_path (char * path, char* name,int pos) {

	int i=0,j=0,k=0;
	int ret;
	
	while ( (path[i]!='\0') && (k<pos) ) {
		j=0;
		while ( (path[i]!='/') && (path[i]!='\0') ) {
			name[j]=path[i];
			i++;
			j++;
		}
		if (path[i]=='/')
			i++;
		name[j]='\0';
		k++;
	}
	if (k==pos) 
		ret=k;
	else
		ret=0;
	return ret;
}

int get_path_length (char *path) {
	int i=0,k=0;
	while ( path[i]!='\0') {
		if (path[i]=='/')
			k++;
		i++;
	}
	return k;
}

int get_filename_from_path (char * path, char * filename) {
	int path_length;
	path_length = get_path_length(path);
	get_dirname_from_path(path,filename,path_length+1);
	kprintf("%s\n",filename);
	return 0;
}

//directory_t dir;
void fat_open_file (char * path, open_file_descriptor * ofd) {
	int i, path_length;
	char dir_name[14];
	char file_name[14];
	directory_t dir;
	
	path_length = get_path_length(path);
	
	if (path_length==1) {
		open_root_dir(&dir);
	}
	else {
		for(i=1;i<=path_length;i++) {
			get_dirname_from_path(path,dir_name,i);
			printf("%s",dir_name);
			if (strcmp(dir_name,"fd0:")==0) {
				open_root_dir(&dir);
			}
			else {
				open_next_dir(&dir,&dir,dir_name);
			}
		}
	}
	
	get_filename_from_path(path,file_name);
	for(i=0;i<(dir.total_entries);i++) {
		if(strcmp(dir.entry_name[i],file_name)==0) {
			ofd->first_cluster = dir.entry_cluster[i];
			ofd->file_size = dir.entry_size[i];
			break;
		}
	}
	ofd->current_cluster = ofd->first_cluster;
	ofd->current_octet = 0;
	ofd->current_octet_buf = 0;
	ofd->write = write_file;
	ofd->read = read_file;
	ofd->seek = seek_file;
	read_cluster((char*)ofd->buffer,ofd->current_cluster);
}

int seek_file (open_file_descriptor * ofd, long offset, int whence) {
//TODO: ajouter détection d'erreurs.
	switch (whence) {
		case SEEK_SET: // depuis le debut du fichier
			ofd->current_octet = offset;
			break;
		case SEEK_CUR:
			ofd->current_octet += offset;
			break;
		case SEEK_END:
//			ofd->current_octet =  // TODO !
			break;
	}
	ofd->current_octet_buf = 0;

	return 0;
}

size_t write_file (open_file_descriptor * ofd, const void * buf, size_t nb_octet) {
	size_t i;
	for (i=0;i<nb_octet;i++) {
		ofd->buffer[ofd->current_octet_buf]=((char*)buf)[i];
		ofd->current_octet_buf++;
		ofd->current_octet++;
		if (ofd->current_octet_buf>=512) {
			write_cluster(ofd->buffer, ofd->current_cluster);
			ofd->current_cluster = file_alloc_table[ofd->current_cluster]; 
			// Gerer le fait qu'on doit allouer un nouveau cluster si c'est le dernier
			// i.e. si ofd->current_cluster=0xFFF
			ofd->current_octet_buf=0;
		}
	}
	return i;
}

size_t read_file (open_file_descriptor * ofd, void * buf, size_t count) {
	int ret = count;
	int j = 0;
	
	while(count--) { 
		if (ofd->current_octet==ofd->file_size) {
			return 0;
		} else {
			char c = ofd->buffer[ofd->current_octet_buf];
			ofd->current_octet_buf++;
			ofd->current_octet++;
			((char*)buf)[j] = c;
			if (ofd->current_octet_buf>=512) {
				ofd->current_cluster = file_alloc_table[ofd->current_cluster];
				read_cluster((char*)ofd->buffer, ofd->current_cluster);
				ofd->current_octet_buf=0;
			}
		}
	}

	return ret;
}


void init_path () {
	path.current = 0;
	open_root_dir(&(path.dir_list[path.current]));
}

void mount_FAT12 () {
	fat_BS_t boot_sector;
	
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
	
	read_fat(file_alloc_table);
	init_path();
	
}

//TODO: move vers le shell...
void change_dir (char * name) {
		
		int errorcode;

		if ( strcmp(name,"..") == 0 ) {
			if ( path.current!=0 )
				path.current--;
		}
		else {
			errorcode = open_next_dir(&(path.dir_list[path.current]),&(path.dir_list[path.current+1]),name);
			if ( errorcode == 0 )
				path.current++;
			else if (errorcode == 1) 
				printf("\ncd: %s aucun fichier ou dossier de ce type\n",name);
			else if (errorcode == 2) 
				printf("\ncd: %s n'est pas un dossier\n",name);
			else 
				printf("\ncd: erreur inconnue..\n");
		}
		
}

void list_segments (int mode) {
	int i;
	//char name[14]; Inutilisé
	if (mode == 1) 	
		printf("TYPE\tNAME\t\tSIZE\tCLUSTER\n");
	for (i=0;i<path.dir_list[path.current].total_entries;i++) {
			if (mode == 1 ) {
				if ( (path.dir_list[path.current].entry_attributes[i] & 0x10) == 0x10)
					printf("dir\t%s\t\t%d\t%d\n",
								path.dir_list[path.current].entry_name[i],
								path.dir_list[path.current].entry_size[i],
								path.dir_list[path.current].entry_cluster[i]);
				else
					printf("file\t%s\t\t%d\t%d\n",
								path.dir_list[path.current].entry_name[i],
								path.dir_list[path.current].entry_size[i],
								path.dir_list[path.current].entry_cluster[i]);
		  }
			else
				printf("%s ",path.dir_list[path.current].entry_name[i]);
	}
	if (mode == 1)
		printf("total: %d\n",path.dir_list[path.current].total_entries);
}

void print_working_dir () {
	int i;
	for (i=0;i<path.current+1;i++) {
			printf("%s/",path.dir_list[i].name);
	}
}

void print_Boot_Sector () {
/*	boot_sector.oem_name[7] = '\0';
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
	*/
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
