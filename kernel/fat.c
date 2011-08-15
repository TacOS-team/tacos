/**
 * @file fat.c
 *
 * @author TacOS developers 
 *
 * Maxime Cheramy <maxime81@gmail.com>
 * Nicolas Floquet <nicolasfloquet@gmail.com>
 * Benjamin Hautbois <bhautboi@gmail.com>
 * Ludovic Rigal <ludovic.rigal@gmail.com>
 * Simon Vernhes <simon@vernhes.eu>
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

#include <stdio.h>
#include <floppy.h>
#include <types.h>
#include <string.h>
#include <kfat.h>
#include <fcntl.h>
#include <debug.h>
#include <errno.h>
#include <kmalloc.h>
#include <klog.h>

static fat_info_t fat_info;


// Floppy...
addr_CHS_t get_CHS_from_LBA(addr_LBA_t sector_LBA) {
	addr_CHS_t ret;
	ret.Cylinder = sector_LBA / (fat_info.BS.sectors_per_track
			* fat_info.BS.head_side_count);
	ret.Head = (sector_LBA % (fat_info.BS.sectors_per_track
			* fat_info.BS.head_side_count)) / (fat_info.BS.sectors_per_track);
	ret.Sector = (sector_LBA % (fat_info.BS.sectors_per_track
			* fat_info.BS.head_side_count)) % (fat_info.BS.sectors_per_track);
	return ret;
}

/*
 * Read the File Allocation Table.
 */
static void read_fat() {
  uint8_t buffer[fat_info.BS.bytes_per_sector * fat_info.table_size];
  
  uint32_t i;
  int p = 0;
  uint32_t tmp = 0;

// XXX: floppy.
	addr_CHS_t sector_addr;
	char * p_buffer = (char*) buffer;
	for (i = 0; i < fat_info.table_size; i++) {
		sector_addr = get_CHS_from_LBA((fat_info.addr_fat[0] / fat_info.BS.bytes_per_sector) + i);
		floppy_read_sector(sector_addr.Cylinder, sector_addr.Head,
				sector_addr.Sector, p_buffer);
		p_buffer += fat_info.BS.bytes_per_sector;
	}

  if (fat_info.fat_type == FAT12) {
    // decodage FAT12
    for (i = 0; i < fat_info.total_data_clusters; i += 2) {
      tmp = buffer[p] + (buffer[p + 1] << 8) + (buffer[p + 2] << 16);

      // on extrait les 2 clusters de 12bits
      fat_info.file_alloc_table[i] = (tmp & 0xFFF); // 12 least significant bits
      fat_info.file_alloc_table[i + 1] = (tmp >> 12); // 12 most significant bits

      p += 3;
    }
  } else if (fat_info.fat_type == FAT16) {
    for (i = 0; i < fat_info.total_data_clusters; i++) {
      fat_info.file_alloc_table[i] = buffer[i * 2] + (buffer[i * 2 + 1] << 8);
    }
  } else if (fat_info.fat_type == FAT32) {
    for (i = 0; i < fat_info.total_data_clusters; i++) {
      fat_info.file_alloc_table[i] = buffer[i * 4] + (buffer[i * 4 + 1] << 8) + (buffer[i * 4 + 2] << 16) + (buffer[i * 4 + 3] << 24);
    }
  }
}

/*
 * Init the FAT driver for a specific devide.
 */
void mount_FAT() {
  uint8_t sector[512];
	floppy_read_sector(0, 0, 0, (char*) sector);
  memcpy(&fat_info.BS, sector, sizeof(fat_BS_t));

  if (fat_info.BS.table_size_16 == 0) { // Si 0 alors on considère qu'on est en FAT32.
    fat_info.ext_BIOS_16 = NULL;
    fat_info.ext_BIOS_32 = kmalloc(sizeof(fat_extended_BIOS_32_t));

    memcpy(fat_info.ext_BIOS_32, sector + sizeof(fat_BS_t), sizeof(fat_extended_BIOS_32_t));
    
    fat_info.table_size = fat_info.ext_BIOS_32->table_size_32;
  } else {
    fat_info.ext_BIOS_32 = NULL;
    fat_info.ext_BIOS_16 = kmalloc(sizeof(fat_extended_BIOS_16_t));

    memcpy(fat_info.ext_BIOS_16, sector + sizeof(fat_BS_t), sizeof(fat_extended_BIOS_16_t));

    fat_info.table_size = fat_info.BS.table_size_16;
  }
/*
    fprintf(stderr, "table size : %d\n", fat_info.table_size);
    fprintf(stderr, "%d bytes per logical sector\n", fat_info.BS.bytes_per_sector);
    fprintf(stderr, "%d bytes per clusters\n", fat_info.BS.bytes_per_sector * fat_info.BS.sectors_per_cluster);
 */
  fat_info.addr_fat = (unsigned int*) kmalloc(sizeof(unsigned int) * fat_info.BS.table_count);
  
  int i;
  for (i = 0; i < fat_info.BS.table_count; i++) {
    fat_info.addr_fat[i] = (fat_info.BS.reserved_sector_count + i * fat_info.table_size) * fat_info.BS.bytes_per_sector;
  }
  fat_info.addr_root_dir = (fat_info.BS.reserved_sector_count + fat_info.BS.table_count * fat_info.table_size) * fat_info.BS.bytes_per_sector;
  fat_info.addr_data = fat_info.addr_root_dir + (fat_info.BS.root_entry_count * 32);
  if (fat_info.BS.total_sectors_16 > 0)
    fat_info.total_data_clusters = fat_info.BS.total_sectors_16 / fat_info.BS.sectors_per_cluster - fat_info.addr_data / (fat_info.BS.bytes_per_sector * fat_info.BS.sectors_per_cluster);
  else
    fat_info.total_data_clusters = fat_info.BS.total_sectors_32 / fat_info.BS.sectors_per_cluster - fat_info.addr_data / (fat_info.BS.bytes_per_sector * fat_info.BS.sectors_per_cluster);

  if (fat_info.total_data_clusters < 4086) {
    fat_info.fat_type = FAT12;
//      fprintf(stderr, "FAT Type : FAT12\n");
  } else if (fat_info.total_data_clusters < 65526) {
    fat_info.fat_type = FAT16;
//      fprintf(stderr, "FAT Type : FAT16\n");
  } else {
    fat_info.fat_type = FAT32;
//      fprintf(stderr, "FAT Type : FAT32\n");
  }

/*    fprintf(stderr, "First FAT starts at byte %u (sector %u)\n", fat_info.addr_fat[0], fat_info.addr_fat[0] / fat_info.BS.bytes_per_sector);
    fprintf(stderr, "Root directory starts at byte %u (sector %u)\n", fat_info.addr_root_dir, fat_info.addr_root_dir / fat_info.BS.bytes_per_sector);
    fprintf(stderr, "Data area starts at byte %u (sector %u)\n", fat_info.addr_data, fat_info.addr_data / fat_info.BS.bytes_per_sector);
    fprintf(stderr, "Total clusters : %d\n", fat_info.total_data_clusters);
*/

  fat_info.file_alloc_table = (unsigned int*) kmalloc(sizeof(unsigned int) * fat_info.total_data_clusters);

  read_fat();
}

static int last_cluster() {
  if (fat_info.fat_type == FAT12) {
    return 0xFFF;
  } else if (fat_info.fat_type == FAT16) {
    return 0xFFFF;
  } else {
    return 0x0FFFFFFF;
  }
}

static int is_free_cluster(int cluster) {
  return cluster == 0;
}

static int is_last_cluster(int cluster) {
  if (fat_info.fat_type == FAT12) {
    return cluster >= 0xFF8 && cluster <= 0xFFF;
  } else if (fat_info.fat_type == FAT16) {
    return cluster >= 0xFFF8 && cluster <= 0xFFFF;
  } else {
    return cluster >= 0x0FFFFFF8 && cluster <= 0x0FFFFFFF;
  }
}

static int is_used_cluster(int cluster) {
  if (fat_info.fat_type == FAT12) {
    return cluster >= 0x002 && cluster <= 0xFEF;
  } else if (fat_info.fat_type == FAT16) {
    return cluster >= 0x0002 && cluster <= 0xFFEF;
  } else {
    return cluster >= 0x00000002 && cluster <= 0x0FFFFFEF;
  }
}



char * decode_long_file_name(char * name, lfn_entry_t * long_file_name) {

	name[0] = long_file_name->filename1[0];
	name[1] = long_file_name->filename1[2];
	name[2] = long_file_name->filename1[4];
	name[3] = long_file_name->filename1[6];
	name[4] = long_file_name->filename1[8];

	name[5] = long_file_name->filename2[0];
	name[6] = long_file_name->filename2[2];
	name[7] = long_file_name->filename2[4];
	name[8] = long_file_name->filename2[6];
	name[9] = long_file_name->filename2[8];
	name[10] = long_file_name->filename2[10];

	name[11] = long_file_name->filename3[0];
	name[12] = long_file_name->filename3[2];

	name[13] = '\0';

	return name;
}

static time_t convert_datetime_fat_to_time_t(fat_date_t *date, fat_time_t *time) {
  int seconds, minutes, hours;

  if (time) {
    seconds = time->seconds2 * 2;
    minutes = time->minutes;
    hours = time->hours;
  } else {
    seconds = minutes = hours = 0;
  }

  struct tm t = {
    .tm_sec = seconds,
    .tm_min = minutes,
    .tm_hour = hours,
    .tm_mday = date->day,
    .tm_mon = date->month - 1,
    .tm_year = date->year + 80,
  };

  return mktime(&t);
}

static void fat_dir_entry_to_directory_entry(char *filename, fat_dir_entry_t *dir, directory_entry_t *entry) {
  strcpy(entry->name, filename);
  entry->cluster = dir->cluster_pointer;
  entry->attributes = dir->file_attributes;
  entry->size = dir->file_size;
  entry->access_time = 
      convert_datetime_fat_to_time_t(&dir->last_access_date, NULL);
  entry->modification_time =
      convert_datetime_fat_to_time_t(&dir->last_modif_date, &dir->last_modif_time);
  entry->creation_time = 
      convert_datetime_fat_to_time_t(&dir->create_date, &dir->create_time);
}

static void read_data(void * buf, size_t count, int offset) {
  int offset_sector = offset / fat_info.BS.bytes_per_sector;
  int offset_in_sector = offset % fat_info.BS.bytes_per_sector;
  addr_CHS_t sector_addr;
  size_t c;
  char * sector = kmalloc(sizeof(fat_info.BS.bytes_per_sector));

  while (count) {
	  sector_addr = get_CHS_from_LBA(offset_sector);
	  floppy_read_sector(sector_addr.Cylinder, sector_addr.Head,
		  	sector_addr.Sector, sector);
    c = fat_info.BS.bytes_per_sector - offset_in_sector;
    if (count <= c) {
      memcpy(buf, sector + offset_in_sector, count);
      count = 0;
    } else {
      memcpy(buf, sector + offset_in_sector, c);
      count -= c;
      buf += c;
    }
  }
}

static directory_entry_t * decode_lfn_entry(lfn_entry_t* fdir) {
  int j;
  char filename[256];
  uint8_t i_filename = 0;
  uint8_t seq = fdir->seq_number - 0x40;
  for (j = seq-1; j >= 0; j--) {
    decode_long_file_name(filename + i_filename, &fdir[j]);
    i_filename += 13;
  }
  directory_entry_t *dir_entry = kmalloc(sizeof(directory_entry_t));
  fat_dir_entry_to_directory_entry(filename, (fat_dir_entry_t*)&fdir[seq], dir_entry);
  return dir_entry;
}

static directory_entry_t * decode_sfn_entry(fat_dir_entry_t *fdir) {
  int j;
  char filename[256];

  // Copy basis name.
  for (j = 0; j < 8; j++) {
    filename[j] = fdir->utf8_short_name[j];
  }
  
  // Remove trailing whitespaces.
  j = 7;
  while (j >= 0 && filename[j] == ' ') {
    filename[j] = '\0';
    j--;
  }

  // Copy extension.
  filename[8] = '.';
  for (j = 0; j < 3; j++) {
    filename[9 + j] = fdir->file_extension[j];
  }

  // Remove trailing whitespaces in extension.
  j = 2;
  while (j >= 0 && filename[9 + j] == ' ') {
    filename[9 + j] = '\0';
    j--;
  }

  directory_entry_t *dir_entry = kmalloc(sizeof(directory_entry_t));
  fat_dir_entry_to_directory_entry(filename, fdir, dir_entry);
  return dir_entry;
}


static void read_dir_entries(fat_dir_entry_t *fdir, directory_t *dir, int n) {
  int i;
  for (i = 0; i < n && fdir[i].utf8_short_name[0]; i++) {
    directory_entry_t * dir_entry;

    if (fdir[i].utf8_short_name[0] != 0xE5) {
      if (fdir[i].file_attributes == 0x0F && ((lfn_entry_t*) &fdir[i])->seq_number & 0x40) {
        dir_entry = decode_lfn_entry((lfn_entry_t*) &fdir[i]);
        uint8_t seq = ((lfn_entry_t*) &fdir[i])->seq_number - 0x40;
        i += seq;
      } else {
        dir_entry = decode_sfn_entry(&fdir[i]);
      }
      dir_entry->next = dir->entries;
      dir->entries = dir_entry;
      dir->total_entries++;
    }
  }
}

static void open_dir(int cluster, directory_t *dir) {
  int n_clusters = 0;
  int next = cluster;
  while (!is_last_cluster(next)) {
    next = fat_info.file_alloc_table[next];
    n_clusters++;
  }

  int n_dir_entries = fat_info.BS.bytes_per_sector * fat_info.BS.sectors_per_cluster / sizeof(fat_dir_entry_t);
  fat_dir_entry_t * sub_dir = kmalloc(n_dir_entries * sizeof(fat_dir_entry_t) * n_clusters);

  int c = 0;
  next = cluster;
  while (!is_last_cluster(next)) {
    read_data(sub_dir + c * n_dir_entries, n_dir_entries * sizeof(fat_dir_entry_t), fat_info.addr_data + (next - 2) * fat_info.BS.sectors_per_cluster * fat_info.BS.bytes_per_sector);
    next = fat_info.file_alloc_table[next];
    c++;
  }

  dir->cluster = cluster;
  dir->total_entries = 0;
  dir->entries = NULL;

  read_dir_entries(sub_dir, dir, n_dir_entries * n_clusters);
}

static directory_t * open_root_dir() {
  directory_t *dir = kmalloc(sizeof(directory_t));

  if (fat_info.fat_type == FAT32) {
    open_dir(fat_info.ext_BIOS_32->cluster_root_dir, dir);
  } else {
    fat_dir_entry_t *root_dir = kmalloc(sizeof(fat_dir_entry_t) * fat_info.BS.root_entry_count);
  
    read_data(root_dir, sizeof(fat_dir_entry_t) * fat_info.BS.root_entry_count, fat_info.addr_root_dir);
  
    dir->cluster = -1;
    dir->total_entries = 0;
    dir->entries = NULL;
  
    read_dir_entries(root_dir, dir, fat_info.BS.root_entry_count);
  
    kfree(root_dir);
  }

  return dir;
}

static int open_next_dir(directory_t * prev_dir, directory_t * next_dir, char * name) {
  int next = 0;

  directory_entry_t *dentry = prev_dir->entries;
  while (dentry) {
    if (strcmp(dentry->name, name) == 0) {
      if ((dentry->attributes & 0x10) == 0x10) { //c'est bien un repe
        next = dentry->cluster;
        break;
      } else {
        return 2;
      }
    }
    dentry = dentry->next;
  }

  if (next == 0) {
    return 1;
  }
 
  open_dir(next, next_dir);

  return 0;
}

static void split_dir_filename(const char * path, char * dir, char * filename) {
  char *p = strrchr(path, '/');
  strcpy(filename, p+1);
  for (; path < p; path++, dir++) {
    *dir = *path;
  } 
  *dir = '\0';
}

static directory_t * open_dir_from_path(const char *path) {

  if (path[0] == '\0' || strcmp(path, "/") == 0)
    return open_root_dir();

  // Only absolute paths.
  if (path[0] != '/')
    return NULL;

  char buf[256];
  int i = 1;
  while (path[i] == '/')
    i++;

  directory_t * dir = open_root_dir();

  int j = 0;
  do {
    if (path[i] == '/' || path[i] == '\0') {
      buf[j] = '\0';

      if (j > 0 && open_next_dir(dir, dir, buf) != 0)
        return NULL;

      j = 0;
    } else {
      buf[j] = path[i];
      j++;
    }
  } while (path[i++] != '\0');

  return dir;
}

static directory_entry_t * open_file_from_path(const char *path) {
  char * dir = kmalloc(strlen(path));
  char filename[256];
  split_dir_filename(path, dir, filename);

  directory_t * directory = open_dir_from_path(dir);
  kfree(dir);

  directory_entry_t *dir_entry = directory->entries;
  while (dir_entry) {
    if (strcmp(dir_entry->name, filename) == 0) {
      kfree(directory);
      // TODO: free de la liste chainée.
      return dir_entry;
    }
    dir_entry = dir_entry->next;
  }

  kfree(directory);
  // TODO: free de la liste chainée.
  
  return NULL;
}










//directory_t dir;
int fat_open_file(char * path, open_file_descriptor * ofd, uint32_t flags) {
	return 0;
}

// TODO: Est-ce que offset peut être négatif ? Si oui, le gérer.
int seek_file(open_file_descriptor * ofd, long offset, int whence) {
	return 0;
}

size_t write_file(open_file_descriptor * ofd, const void * buf, size_t nb_octet) {
	return 0;
}

size_t read_file(open_file_descriptor * ofd, void * buf, size_t count) {
	return 0;
}

//
// TODO: move vers le shell...
//

// void init_path() {
// 	path.current = 0;
// 	open_root_dir(&(path.dir_list[path.current]));
// }
// 
// 
// void change_dir(char * name) {
// 
// 	int errorcode;
// 
// 	if (strcmp(name, "..") == 0) { //XXX: Beurk. Normalement ".." est un dossier disponible...
// 		if (path.current != 0)
// 			path.current--;
// 	} else {
// 		errorcode = open_next_dir(&(path.dir_list[path.current]),
// 				&(path.dir_list[path.current + 1]), name);
// 		if (errorcode == 0)
// 			path.current++;
// 		else if (errorcode == 1)
// 			printf("\ncd: %s aucun fichier ou dossier de ce type\n", name);
// 		else if (errorcode == 2)
// 			printf("\ncd: %s n'est pas un dossier\n", name);
// 		else
// 			printf("\ncd: erreur inconnue..\n");
// 	}
// 
// }
// 
// void list_segments(int mode) {
// 	int i;
// 	//char name[14]; Inutilisé
// 	if (mode == 1)
// 		printf("TYPE\tNAME\t\tSIZE\tCLUSTER\n");
// 	for (i = 0; i < path.dir_list[path.current].total_entries; i++) {
// 		if (mode == 1) {
// 			if ((path.dir_list[path.current].entry_attributes[i] & 0x10)
// 					== 0x10)
// 				printf("dir\t%s\t\t%d\t%d\n",
// 						path.dir_list[path.current].entry_name[i],
// 						path.dir_list[path.current].entry_size[i],
// 						path.dir_list[path.current].entry_cluster[i]);
// 			else
// 				printf("file\t%s\t\t%d\t%d\n",
// 						path.dir_list[path.current].entry_name[i],
// 						path.dir_list[path.current].entry_size[i],
// 						path.dir_list[path.current].entry_cluster[i]);
// 		} else
// 			printf("%s ", path.dir_list[path.current].entry_name[i]);
// 	}
// 	if (mode == 1)
// 		printf("total: %d\n", path.dir_list[path.current].total_entries);
// }
// 
// void print_working_dir() {
// 	int i;
// 	for (i = 0; i < path.current + 1; i++) {
// 		printf("%s/", path.dir_list[i].name);
// 	}
// }
//

void strrchr() {

}

void change_dir(char * name) {

}

void print_working_dir() {

}

void list_segments(int mode) {

}

void print_Boot_Sector() {

}
