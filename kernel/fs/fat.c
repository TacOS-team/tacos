/**
 * @file fat.c
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

#include <ctype.h>
//#include <errno.h>
#include <fcntl.h>
#include <fs/fat.h>
#include <klog.h>
#include <kmalloc.h>
#include <string.h>
#include <types.h>
#include <clock.h>
#include <kdirent.h>

#ifndef EOF
# define EOF (-1)
#endif

typedef struct _fat_fs_instance_t {
	fs_instance_t super;
	fat_info_t fat_info;
} fat_fs_instance_t;

/*
 * Read the File Allocation Table.
 */
static void read_fat(fat_fs_instance_t *instance) {
	uint8_t buffer[instance->fat_info.BS.bytes_per_sector * instance->fat_info.table_size];
	
	uint32_t i;
	int p = 0;
	uint32_t tmp = 0;

  instance->fat_info.read_data(instance->super.device, buffer, sizeof(buffer), instance->fat_info.addr_fat[0]);

	if (instance->fat_info.fat_type == FAT12) {
		// decodage FAT12
		for (i = 0; i < instance->fat_info.total_data_clusters; i += 2) {
			tmp = buffer[p] + (buffer[p + 1] << 8) + (buffer[p + 2] << 16);

			// on extrait les 2 clusters de 12bits
			instance->fat_info.file_alloc_table[i] = (tmp & 0xFFF); // 12 least significant bits
			instance->fat_info.file_alloc_table[i + 1] = (tmp >> 12); // 12 most significant bits

			p += 3;
		}
	} else if (instance->fat_info.fat_type == FAT16) {
		for (i = 0; i < instance->fat_info.total_data_clusters; i++) {
			instance->fat_info.file_alloc_table[i] = buffer[i * 2] + (buffer[i * 2 + 1] << 8);
		}
	} else if (instance->fat_info.fat_type == FAT32) {
		for (i = 0; i < instance->fat_info.total_data_clusters; i++) {
			instance->fat_info.file_alloc_table[i] = buffer[i * 4] + (buffer[i * 4 + 1] << 8) + (buffer[i * 4 + 2] << 16) + (buffer[i * 4 + 3] << 24);
		}
	}
}

/*
static void write_fat() {
  uint8_t buffer[fat_info.BS.bytes_per_sector * fat_info.table_size];
  
  uint32_t i;
  int p = 0;
  uint32_t tmp = 0;

  if (fat_info.fat_type == FAT12) {
    for (i = 0; i < fat_info.total_data_clusters; i += 2) {
      tmp = (fat_info.file_alloc_table[i + 1] << 12) + (fat_info.file_alloc_table[i] & 0xFFF);
      buffer[p++] = tmp & 0x0000FF;
      buffer[p++] = tmp & 0x00FF00;
      buffer[p++] = tmp & 0xFF0000;
    }
  } else if (fat_info.fat_type == FAT16) {
    for (i = 0; i < fat_info.total_data_clusters; i++) {
      buffer[i*2] = fat_info.file_alloc_table[i] & 0x00FF;
      buffer[i*2 + 1] = fat_info.file_alloc_table[i] & 0xFF00;
    }
  } else if (fat_info.fat_type == FAT32) {
    for (i = 0; i < fat_info.total_data_clusters; i++) {
      buffer[i*4]     = fat_info.file_alloc_table[i] & 0x000000FF;
      buffer[i*4 + 1] = fat_info.file_alloc_table[i] & 0x0000FF00;
      buffer[i*4 + 2] = fat_info.file_alloc_table[i] & 0x00FF0000;
      buffer[i*4 + 3] = fat_info.file_alloc_table[i] & 0xFF000000;
    }
  }
 
  for (i = 0; i < fat_info.BS.table_count; i++) {
    fat_info.write_data(instance->super.device, buffer, sizeof(buffer), fat_info.addr_fat[i]);
  }
}
*/

static void write_fat_entry(fat_fs_instance_t *instance, int index) {
	int i;
	if (instance->fat_info.fat_type == FAT12) {
		uint32_t tmp;
		uint8_t buffer[3]; // 24 bits : 2 entries
		if (index % 2 == 0) {
      tmp = (instance->fat_info.file_alloc_table[index + 1] << 12) + (instance->fat_info.file_alloc_table[index] & 0xFFF);
		} else {
      tmp = (instance->fat_info.file_alloc_table[index] << 12) + (instance->fat_info.file_alloc_table[index - 1] & 0xFFF);
		}
    buffer[0] = tmp & 0x0000FF;
    buffer[1] = tmp & 0x00FF00;
    buffer[2] = tmp & 0xFF0000;

		for (i = 0; i < instance->fat_info.BS.table_count; i++) {
			instance->fat_info.write_data(instance->super.device, buffer, sizeof(buffer), instance->fat_info.addr_fat[i] + index * 12);
		}
  } else if (instance->fat_info.fat_type == FAT16) {
		uint8_t buffer[2];
    buffer[0] = instance->fat_info.file_alloc_table[index] & 0x00FF;
    buffer[1] = instance->fat_info.file_alloc_table[index] & 0xFF00;
		for (i = 0; i < instance->fat_info.BS.table_count; i++) {
			instance->fat_info.write_data(instance->super.device, buffer, sizeof(buffer), instance->fat_info.addr_fat[i] + index * 16);
		}
  } else if (instance->fat_info.fat_type == FAT32) {
		uint8_t buffer[4];
    buffer[0] = instance->fat_info.file_alloc_table[index] & 0x000000FF;
    buffer[1] = instance->fat_info.file_alloc_table[index] & 0x0000FF00;
    buffer[2] = instance->fat_info.file_alloc_table[index] & 0x00FF0000;
    buffer[3] = instance->fat_info.file_alloc_table[index] & 0xFF000000;
		for (i = 0;  i < instance->fat_info.BS.table_count; i++) {
			instance->fat_info.write_data(instance->super.device, buffer, sizeof(buffer), instance->fat_info.addr_fat[i] + index * 32);
		}
  }
}

/*
 * Init the FAT driver for a specific devide.
 */
fs_instance_t* mount_FAT(open_file_descriptor* ofd) {
	klog("mounting FAT");

	fat_fs_instance_t *instance = kmalloc(sizeof(fat_fs_instance_t));
	instance->super.open = fat_open_file;
	instance->super.mkdir = fat_mkdir;
	instance->super.readdir = fat_readdir;
	instance->super.opendir = fat_opendir;
	instance->super.stat = fat_stat;
	instance->super.unlink = fat_unlink;

	instance->fat_info.read_data = ((blkdev_interfaces*)(ofd->extra_data))->read;
	instance->fat_info.write_data = ((blkdev_interfaces*)(ofd->extra_data))->write;

	uint8_t sector[512];
	instance->fat_info.read_data(instance->super.device, sector, 512, 0);
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

	return (fs_instance_t*)instance;
}

void umount_FAT(fs_instance_t *instance) {
	kfree(instance);
}

static void encode_long_file_name(char * name, lfn_entry_t * long_file_name, int n_entries) {
 // TODO: Checksum.
  long_file_name[0].seq_number = 0x40 + n_entries;
  int i, j;
  int last = 0;
  for (i = n_entries - 1; i >= 0; i--) {
    long_file_name[i].attributes = 0x0f;
    if (i != n_entries - 1)
      long_file_name[i].seq_number = i + 1;
    for (j = 0; j < 5; j++) {
      if (last) {
        long_file_name[i].filename1[j * 2] = 0xFF;
        long_file_name[i].filename1[j * 2 + 1] = 0xFF;
      } else if (name[j] != '\0') {
        long_file_name[i].filename1[j * 2] = name[i * 13 + j];
        long_file_name[i].filename1[j * 2 + 1] = 0;
      } else {
        long_file_name[i].filename1[j * 2] = 0;
        long_file_name[i].filename1[j * 2 + 1] = 0;
        last = 1;
      } 
    }
    for (j = 5; j < 11; j++) {
      if (last) {
        long_file_name[i].filename2[(j - 5) * 2] = 0xFF;
        long_file_name[i].filename2[(j - 5) * 2 + 1] = 0xFF;
      } else if (name[j] != '\0') {
        long_file_name[i].filename2[(j - 5) * 2] = name[i * 13 + j];
        long_file_name[i].filename2[(j - 5) * 2 + 1] = 0;
      } else {
        long_file_name[i].filename2[(j - 5) * 2] = 0;
        long_file_name[i].filename2[(j - 5) * 2 + 1] = 0;
        last = 1;
      }
    }

    if (last) {
      long_file_name[i].filename3[0] = 0xFF;
      long_file_name[i].filename3[1] = 0xFF;
    } else if (name[11] != '\0') {
      long_file_name[i].filename3[0] = name[i * 13 + 11];
      long_file_name[i].filename3[1] = 0;
    } else {
      long_file_name[i].filename3[0] = 0;
      long_file_name[i].filename3[1] = 0;
      last = 1;
    }

    if (last) {
      long_file_name[i].filename3[0] = 0xFF;
      long_file_name[i].filename3[1] = 0xFF;
    } else if (name[12] != '\0') {
      long_file_name[i].filename3[0] = name[i * 13 + 12];
      long_file_name[i].filename3[1] = 0;
    } else {
      long_file_name[i].filename3[0] = 0;
      long_file_name[i].filename3[1] = 0;
      last = 1;
    }
  }
}

static int last_cluster(fat_fs_instance_t *instance) {
	if (instance->fat_info.fat_type == FAT12) {
		return 0xFFF;
	} else if (instance->fat_info.fat_type == FAT16) {
		return 0xFFFF;
	} else {
		return 0x0FFFFFFF;
	}
}

static int is_free_cluster(int cluster) {
	return cluster == 0;
}

static int is_last_cluster(fat_fs_instance_t *instance, int cluster) {
	if (instance->fat_info.fat_type == FAT12) {
		return cluster >= 0xFF8 && cluster <= 0xFFF;
	} else if (instance->fat_info.fat_type == FAT16) {
		return cluster >= 0xFFF8 && cluster <= 0xFFFF;
	} else {
		return cluster >= 0x0FFFFFF8 && cluster <= 0x0FFFFFFF;
	}
}

/*
static int is_used_cluster(int cluster) {
	if (instance->fat_info.fat_type == FAT12) {
		return cluster >= 0x002 && cluster <= 0xFEF;
	} else if (instance->fat_info.fat_type == FAT16) {
		return cluster >= 0x0002 && cluster <= 0xFFEF;
	} else {
		return cluster >= 0x00000002 && cluster <= 0x0FFFFFEF;
	}
}
*/


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

	return clock_mktime(&t);
}

/*
static void convert_time_t_to_datetime_fat(time_t time, fat_time_t *timefat, fat_date_t *datefat) {
  #define MINUTES 60
  #define HOURS 3600
  #define DAYS 86400

  int days_per_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int days_per_month_leap[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  int year = 1970;
  int month = 0;
  int day = 0;
  int hours = 0;
  int min = 0;
  int sec = 0;

  int secs_year = DAYS * 365;
  int *dpm = days_per_month;

  while (time) {
    if (time >= secs_year) {
      year++;
      time -= secs_year;

      if (!(year % 400 && (year % 100 == 0 || (year & 3)))) {
        secs_year = DAYS * 366;
        dpm = days_per_month_leap;
      } else {
        secs_year = DAYS * 365;
        dpm = days_per_month;
      }
    } else {
      if (time >= dpm[month] * DAYS) {
        time -= dpm[month] * DAYS;
        month++;
      } else {
        day = time / DAYS;
        time -= day * DAYS;
        hours = time / HOURS;
        time -= hours * HOURS;
        min = time / MINUTES;
        time -= min * MINUTES;
        sec = time;
        time = 0;
      }
    }
  }

  datefat->year = year - 1980;
  datefat->month = month + 1;
  datefat->day = day + 1;
  if (timefat) {
    timefat->hours = hours;
    timefat->minutes = min;
    timefat->seconds2 = sec / 2;
  }
}
*/

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

static char * lfn_to_sfn(char * filename) {
//  char * lfn = strdup(filename);
	int len = strlen(filename);
	char* lfn = (char*) kmalloc((len+1)*sizeof(char)); // len + 1 pour le '\0'
	strcpy(lfn, filename);

  char * sfn = kmalloc(12);

  // To upper case.
  int i = 0;
  while (lfn[i] != '\0') {
    lfn[i] = toupper(lfn[i]);
    i++;
  }

  // TODO: Convert to OEM (=> '_').

  // Strip all leading and embedded spaces
  int j = 0;
  i = 0;
  while (lfn[i] != '\0') {
    if (lfn[i] != ' ') {
      lfn[j] = lfn[i];
      j++;
    }
    i++;
  }
  lfn[j] = '\0';

  char * ext = strrchr(lfn, '.');
  int has_ext = (ext != NULL) && (lfn + j - ext - 1 <= 3);

  if (has_ext) {
    // Copy first 8 caracters.
    i = 0;
    j = 0;
    while (&lfn[i] <= ext) {
      if (lfn[i] != '.' && j < 8) {
        sfn[j] = lfn[i];
        j++;
      }
      i++;
    }
  
    // padding
    while (j < 8)
      sfn[j++] = ' ';

    // Copy extension.
    sfn[j++] = '.';
    while (lfn[i] != '\0') {
      sfn[j++] = lfn[i];
      i++;
    }

    while (j < 12)
      sfn[j++] = ' ';
  } else {
    i = 0;
    j = 0;
    while (lfn[i] != '\0' && j < 8) {
      if (lfn[i] != '.') {
        sfn[j] = lfn[i];
        j++;
      }
      i++;
    }
    while (j < 12)
      sfn[j++] = ' ';
  }

  // TODO: numeric-tail generation.

  return sfn;
}

static void decode_short_file_name(char *filename, fat_dir_entry_t *fdir) {
  int j, k;
	int notspace = 0;

  // Copy basis name.
  for (j = 7; j >= 0; j--) {
		if (notspace || fdir->utf8_short_name[j] != ' ') {
			if (!notspace) {
				notspace = j;
			}
			if (fdir->reserved && 0x08) {
				filename[j] = tolower(fdir->utf8_short_name[j]);
			} else {
				filename[j] = fdir->utf8_short_name[j];
			}
		}
  }
  
	notspace++; // notspace est la position du premier caractère != ' '
  filename[notspace++] = '.';

	int notspaceext = -2;
  // Copy extension.
  for (k = 2; k >= 0; k--) {
		if (notspaceext > 0 || fdir->file_extension[k] != ' ') {
			if (notspaceext <= 0) {
				notspaceext = k;
			}
			if (fdir->reserved && 0x10) {
				filename[notspace + k] = tolower(fdir->file_extension[k]);
			} else {
				filename[notspace + k] = fdir->file_extension[k];
			}
		}
  }

	filename[notspace + notspaceext + 1] = '\0';


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
  char filename[256];
	decode_short_file_name(filename, fdir);
	directory_entry_t *dir_entry = kmalloc(sizeof(directory_entry_t));
	fat_dir_entry_to_directory_entry(filename, fdir, dir_entry);
	return dir_entry;
}




static void read_dir_entries(fat_dir_entry_t *fdir, directory_t *dir, int n) {
	int i;
	for (i = 0; i < n && fdir[i].utf8_short_name[0]; i++) {
		directory_entry_t * dir_entry;

		if ((unsigned char)fdir[i].utf8_short_name[0] != 0xE5) {
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

static void open_dir(fat_fs_instance_t *instance, int cluster, directory_t *dir) {
	int n_clusters = 0;
	int next = cluster;
	while (!is_last_cluster(instance, next)) {
		next = instance->fat_info.file_alloc_table[next];
		n_clusters++;
	}

	int n_dir_entries = instance->fat_info.BS.bytes_per_sector * instance->fat_info.BS.sectors_per_cluster / sizeof(fat_dir_entry_t);
	fat_dir_entry_t * sub_dir = kmalloc(n_dir_entries * sizeof(fat_dir_entry_t) * n_clusters);

	int c = 0;
	next = cluster;
	while (!is_last_cluster(instance, next)) {
		instance->fat_info.read_data(instance->super.device, (uint8_t*)(sub_dir + c * n_dir_entries), n_dir_entries * sizeof(fat_dir_entry_t), instance->fat_info.addr_data + (next - 2) * instance->fat_info.BS.sectors_per_cluster * instance->fat_info.BS.bytes_per_sector);
		next = instance->fat_info.file_alloc_table[next];
		c++;
	}

	dir->cluster = cluster;
	dir->total_entries = 0;
	dir->entries = NULL;

	read_dir_entries(sub_dir, dir, n_dir_entries * n_clusters);
}

static directory_t * open_root_dir(fat_fs_instance_t *instance) {
	directory_t *dir = kmalloc(sizeof(directory_t));

	if (instance->fat_info.fat_type == FAT32) {
		open_dir(instance, instance->fat_info.ext_BIOS_32->cluster_root_dir, dir);
	} else {
		fat_dir_entry_t *root_dir = kmalloc(sizeof(fat_dir_entry_t) * instance->fat_info.BS.root_entry_count);
	
		instance->fat_info.read_data(instance->super.device, (uint8_t*)root_dir, sizeof(fat_dir_entry_t) * instance->fat_info.BS.root_entry_count, instance->fat_info.addr_root_dir);
	
		dir->cluster = -1;
		dir->total_entries = 0;
		dir->entries = NULL;
	
		read_dir_entries(root_dir, dir, instance->fat_info.BS.root_entry_count);
	
		kfree(root_dir);
	}

	return dir;
}

static int open_next_dir(fat_fs_instance_t *instance, directory_t * prev_dir, directory_t * next_dir, char * name) {
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
 
	open_dir(instance, next, next_dir);

	return 0;
}

static void split_dir_filename(const char * path, char * dir, char * filename) {
	char *p = strrchr(path, '/');
	strcpy(filename, p+1);
	if (p == path) {
		*dir = '/';
		dir++;
	}
	for (; path < p; path++, dir++) {
		*dir = *path;
	} 
	*dir = '\0';
}

static directory_t * open_dir_from_path(fat_fs_instance_t *instance, const char *path) {
	if (path[0] == '\0' || strcmp(path, "/") == 0)
		return open_root_dir(instance);

	// Only absolute paths.
	if (path[0] != '/')
		return NULL;

	char buf[256];
	int i = 1;
	while (path[i] == '/')
		i++;

	directory_t * dir = open_root_dir(instance);

	int j = 0;
	do {
		if (path[i] == '/' || path[i] == '\0') {
			buf[j] = '\0';

			if (j > 0 && open_next_dir(instance, dir, dir, buf) != 0)
				return NULL;

			j = 0;
		} else {
			buf[j] = path[i];
			j++;
		}
	} while (path[i++] != '\0');

	return dir;
}

static directory_entry_t * open_file_from_path(fat_fs_instance_t *instance, const char *path) {
	char * dir = kmalloc(strlen(path));
	char filename[256];
	split_dir_filename(path, dir, filename);

	directory_t * directory = open_dir_from_path(instance, dir);
	kfree(dir);

	if (directory) {
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
	}
	
	return NULL;
}


static int alloc_cluster(fat_fs_instance_t *instance, int n) {
  if (n <= 0) {
    return last_cluster(instance);
  }
  int next = alloc_cluster(instance, n - 1);
  unsigned int i;
  for (i = 0; i < instance->fat_info.total_data_clusters; i++) {
    if (is_free_cluster(instance->fat_info.file_alloc_table[i])) {
      instance->fat_info.file_alloc_table[i] = next;
			write_fat_entry(instance, i);
      return i;
    }
  }
  return -1;
}

static void init_dir_cluster(fat_fs_instance_t *instance, int cluster) {
  int n_dir_entries = instance->fat_info.BS.bytes_per_sector * instance->fat_info.BS.sectors_per_cluster / sizeof(fat_dir_entry_t);
  fat_dir_entry_t * dir_entries = kmalloc(n_dir_entries * sizeof(fat_dir_entry_t));
	memset(dir_entries, 0, n_dir_entries * sizeof(fat_dir_entry_t));
 
  instance->fat_info.write_data(instance->super.device, (uint8_t*) dir_entries, sizeof(fat_dir_entry_t) * n_dir_entries, instance->fat_info.addr_data + (cluster - 2) * instance->fat_info.BS.sectors_per_cluster * instance->fat_info.BS.bytes_per_sector);
}

static int add_fat_dir_entry(fat_fs_instance_t *instance, char * path, fat_dir_entry_t *fentry, int n) {
  directory_t *dir = open_dir_from_path(instance, path);
  int next = dir->cluster;
  if (next > 0) {
    int n_clusters = 0;
    while (!is_last_cluster(instance, next)) {
      next = instance->fat_info.file_alloc_table[next];
      n_clusters++;
    }
  
    int n_dir_entries = instance->fat_info.BS.bytes_per_sector * instance->fat_info.BS.sectors_per_cluster / sizeof(fat_dir_entry_t);
    fat_dir_entry_t * dir_entries = kmalloc(n_dir_entries * sizeof(fat_dir_entry_t) * n_clusters);
  
    int c = 0;
    next = dir->cluster;
    while (!is_last_cluster(instance, next)) {
      instance->fat_info.read_data(instance->super.device, (uint8_t*)(dir_entries + c * n_dir_entries), n_dir_entries * sizeof(fat_dir_entry_t), instance->fat_info.addr_data + (next - 2) * instance->fat_info.BS.sectors_per_cluster * instance->fat_info.BS.bytes_per_sector);
      next = instance->fat_info.file_alloc_table[next];
      c++;
    }
  
    int i;
    int consecutif = 0;
    for (i = 0; i < n_dir_entries * n_clusters; i++) {
      if (dir_entries[i].utf8_short_name[0] == 0 || (unsigned char)dir_entries[i].utf8_short_name[0] == 0xe5) {
        consecutif++;
        if (consecutif == n) {
          next = dir->cluster;
          int j;
          for (j = 0; j < (i - n + 1) / n_dir_entries; j++)
            next = instance->fat_info.file_alloc_table[next];
          for (j = 0; j < n; j++) {
            int off = (i - n + j + 1) % n_dir_entries; // offset dans le cluster.
            instance->fat_info.write_data(instance->super.device, (uint8_t*)(&fentry[j]), sizeof(fat_dir_entry_t), instance->fat_info.addr_data + (next - 2) * instance->fat_info.BS.sectors_per_cluster * instance->fat_info.BS.bytes_per_sector + off * sizeof(fat_dir_entry_t));
            if (off == n_dir_entries - 1) {
              next = instance->fat_info.file_alloc_table[next];
            }
          }
          return 0;
        }
      } else {
        consecutif = 0;
      }
    }
    if (consecutif < n) {
      int j;
      int newcluster = alloc_cluster(instance, 1);
      init_dir_cluster(instance, newcluster);
      next = dir->cluster;
      while (!is_last_cluster(instance, instance->fat_info.file_alloc_table[next])) {
        next = instance->fat_info.file_alloc_table[next];
      }
      instance->fat_info.file_alloc_table[next] = newcluster;
  
      for (j = 0; j < consecutif; j++) {
        int off = n_dir_entries - consecutif + j;
        instance->fat_info.write_data(instance->super.device, (uint8_t*)(&fentry[j]), sizeof(fat_dir_entry_t), instance->fat_info.addr_data + (next - 2) * instance->fat_info.BS.sectors_per_cluster * instance->fat_info.BS.bytes_per_sector + off * sizeof(fat_dir_entry_t));
      }
      for (j = consecutif; j < n; j++) {
        int off = n_dir_entries - consecutif + j;
        instance->fat_info.write_data(instance->super.device, (uint8_t*)(&fentry[j]), sizeof(fat_dir_entry_t), instance->fat_info.addr_data + (newcluster - 2) * instance->fat_info.BS.sectors_per_cluster * instance->fat_info.BS.bytes_per_sector + off * sizeof(fat_dir_entry_t));
      }
      return 0;
    }
  } else if (instance->fat_info.fat_type != FAT32) {
    int i;
    int consecutif = 0;
    fat_dir_entry_t *root_dir = kmalloc(sizeof(fat_dir_entry_t) * instance->fat_info.BS.root_entry_count);
    instance->fat_info.read_data(instance->super.device, (uint8_t*)root_dir, sizeof(fat_dir_entry_t) * instance->fat_info.BS.root_entry_count, instance->fat_info.addr_root_dir);

    for (i = 0; i < instance->fat_info.BS.root_entry_count; i++) {
      if (root_dir[i].utf8_short_name[0] == 0 || (unsigned char)root_dir[i].utf8_short_name[0] == 0xe5) {
        consecutif++;
        if (consecutif == n) {
          instance->fat_info.write_data(instance->super.device, (uint8_t*)fentry, sizeof(fat_dir_entry_t) * n, instance->fat_info.addr_root_dir + (i - n + 1) * sizeof(fat_dir_entry_t));
          return 0;
        }
      }
    }
  }
  return 1;
}



int fat_opendir(fs_instance_t *instance, const char * path) {
	directory_t *f;

	if ((f = open_dir_from_path((fat_fs_instance_t*)instance, path)) == NULL)
		return -ENOENT;

	kfree(f);

	return 0;
}

int fat_readdir(fs_instance_t *instance, const char * path, int iter, char * filename) {
	directory_t *dir;

	if ((dir = open_dir_from_path((fat_fs_instance_t*)instance, path)) == NULL)
		return -ENOENT;

	directory_entry_t *dir_entry = dir->entries;
	while (dir_entry) {
		if (!iter) {
			strcpy(filename, dir_entry->name);
			
			kfree(dir); // TODO: libérer liste chainée.
			return 0;
		}
		iter--;
		dir_entry = dir_entry->next;
	}

	kfree(dir); // TODO: libérer liste chainée.
	return 1;
}

void load_buffer(open_file_descriptor *ofd) {
	fat_fs_instance_t *instance = (fat_fs_instance_t*) ofd->fs_instance;
	size_t size_buffer = sizeof(ofd->buffer) < instance->fat_info.BS.bytes_per_sector ? 
			sizeof(ofd->buffer) : instance->fat_info.BS.bytes_per_sector;
	int current_octet_cluster = ofd->current_octet % instance->fat_info.bytes_per_cluster;
	size_t r = instance->fat_info.bytes_per_cluster - current_octet_cluster;

	if (r < size_buffer) {
		if (r > 0)
			instance->fat_info.read_data(instance->super.device, ofd->buffer, r, instance->fat_info.addr_data + (ofd->current_cluster - 2) * instance->fat_info.bytes_per_cluster + current_octet_cluster);
	} else {
		instance->fat_info.read_data(instance->super.device, ofd->buffer, size_buffer, instance->fat_info.addr_data + (ofd->current_cluster - 2) * instance->fat_info.bytes_per_cluster + current_octet_cluster);
	}
	ofd->current_octet_buf = 0;
}

// TODO: Est-ce que offset peut être négatif ? Si oui, le gérer.
int fat_seek_file(open_file_descriptor * ofd, long offset, int whence) {
	fat_fs_instance_t *instance = (fat_fs_instance_t*) ofd->fs_instance;
	switch (whence) {
	case SEEK_SET: // depuis le debut du fichier
		if ((unsigned long) offset < ofd->file_size) {
			// On se met au début :
			ofd->current_cluster = ofd->first_cluster;
			ofd->current_octet = 0;

			// On avance de cluster en cluster.
			while (offset >= (long)instance->fat_info.bytes_per_cluster) {
				offset -= instance->fat_info.bytes_per_cluster;
				ofd->current_octet += instance->fat_info.bytes_per_cluster;
				ofd->current_cluster = instance->fat_info.file_alloc_table[ofd->current_cluster];
			}

			ofd->current_octet += offset;
			load_buffer(ofd);
		} else {
			return -1;
		}
		break;
	case SEEK_CUR:
		if (ofd->current_octet + offset < ofd->file_size) {
			ofd->current_octet += offset;

			int current_octet_cluster = ofd->current_octet % instance->fat_info.bytes_per_cluster;

			while (offset + current_octet_cluster >= (long)instance->fat_info.bytes_per_cluster) {
				offset -= instance->fat_info.bytes_per_cluster;
				ofd->current_cluster = instance->fat_info.file_alloc_table[ofd->current_cluster];
			}
			load_buffer(ofd);
		} else {
			return -1;
		}
		break;
	case SEEK_END:
		if ((unsigned long) offset <= ofd->file_size) {
			return fat_seek_file(ofd, ofd->file_size - offset, SEEK_SET);
		} else {
			return -1;
		}
		break;
	}

	return 0;

}



size_t fat_write_file(open_file_descriptor *ofd __attribute__((__unused__)), const void * buf __attribute__((__unused__)), size_t nb_octet __attribute__((__unused__))) {
	return 0;
}

size_t fat_read_file(open_file_descriptor * ofd, void * buf, size_t count) {
	fat_fs_instance_t *instance = (fat_fs_instance_t*) ofd->fs_instance;
	size_t size_buffer = sizeof(ofd->buffer) < instance->fat_info.BS.bytes_per_sector ? 
			sizeof(ofd->buffer) : instance->fat_info.BS.bytes_per_sector;
	int ret = 0;
	int j = 0;
	
	if ((ofd->flags & O_ACCMODE) == O_WRONLY) {
		//errno = EBADF;
		return EOF;
	}
	
	if (ofd->flags & O_DIRECT) {
		//errno = EINVAL;
		return EOF;
	}
	
	while (count--) {
		if (ofd->current_octet == ofd->file_size) {
			break;
		} else {
			char c = ofd->buffer[ofd->current_octet_buf];
			ret++;
			ofd->current_octet_buf++;
			ofd->current_octet++;
			((char*) buf)[j++] = c;
			int current_octet_cluster = ofd->current_octet % instance->fat_info.bytes_per_cluster;
			if (current_octet_cluster == 0) {
				ofd->current_cluster = instance->fat_info.file_alloc_table[ofd->current_cluster];
				load_buffer(ofd);
			} else if (ofd->current_octet_buf >= size_buffer) {
				load_buffer(ofd); 
			}
		}
	}
	
	return ret;
}

int fat_mkdir (fs_instance_t *instance, const char * path, mode_t mode __attribute__((__unused__))) {
  char * dir = kmalloc(strlen(path));
  char filename[256];
  split_dir_filename(path, dir, filename);

  char * sfn = lfn_to_sfn(filename);
  
  int n_entries = 1 + ((strlen(filename) - 1) / 13);
  lfn_entry_t * long_file_name = kmalloc(sizeof(lfn_entry_t) * (n_entries + 1));
  fat_dir_entry_t *fentry = (fat_dir_entry_t*) &long_file_name[n_entries];

  encode_long_file_name(filename, long_file_name, n_entries);

  strncpy(fentry->utf8_short_name, sfn, 8);
  strncpy(fentry->file_extension, sfn + 8, 3);
  fentry->file_attributes = 0x10; //TODO: Utiliser variable mode et des defines.
  fentry->reserved = 0;
  fentry->create_time_ms = 0;
//  time_t t = time(NULL);
//  convert_time_t_to_datetime_fat(t, &(fentry->create_time), &(fentry->create_date));
//  convert_time_t_to_datetime_fat(t, NULL, &(fentry->last_access_date));
  fentry->ea_index = 0; //XXX
//  convert_time_t_to_datetime_fat(t, &(fentry->last_modif_time), &(fentry->last_modif_date));
  fentry->file_size = 0;
  fentry->cluster_pointer = alloc_cluster((fat_fs_instance_t*)instance, 1);
  init_dir_cluster((fat_fs_instance_t*)instance, fentry->cluster_pointer);

  add_fat_dir_entry((fat_fs_instance_t*)instance, dir, (fat_dir_entry_t*)long_file_name, n_entries + 1);
	kfree(dir);

  return 0;
}

int fat_createfile (fat_fs_instance_t* instance, const char * path, mode_t mode __attribute__((__unused__))) {
  char * dir = kmalloc(strlen(path));
  char filename[256];
  split_dir_filename(path, dir, filename);

  char * sfn = lfn_to_sfn(filename);
  
  int n_entries = 1 + ((strlen(filename) - 1) / 13);
  lfn_entry_t * long_file_name = kmalloc(sizeof(lfn_entry_t) * (n_entries + 1));
  fat_dir_entry_t *fentry = (fat_dir_entry_t*) &long_file_name[n_entries];

  encode_long_file_name(filename, long_file_name, n_entries);

  strncpy(fentry->utf8_short_name, sfn, 8);
  strncpy(fentry->file_extension, sfn + 8, 3);
  fentry->file_attributes = 0x0; //TODO: Utiliser variable mode et des defines.
  fentry->reserved = 0;
  fentry->create_time_ms = 0;
//  time_t t = time(NULL);
//  convert_time_t_to_datetime_fat(t, &(fentry->create_time), &(fentry->create_date));
//  convert_time_t_to_datetime_fat(t, NULL, &(fentry->last_access_date));
  fentry->ea_index = 0; //XXX
//  convert_time_t_to_datetime_fat(t, &(fentry->last_modif_time), &(fentry->last_modif_date));
  fentry->file_size = 0;
  fentry->cluster_pointer = alloc_cluster(instance, 1);
  init_dir_cluster(instance, fentry->cluster_pointer);

  add_fat_dir_entry(instance, dir, (fat_dir_entry_t*)long_file_name, n_entries + 1);
	kfree(dir);

  return 0;
}

open_file_descriptor * fat_open_file(fs_instance_t *instance, const char * path, uint32_t flags) {
	directory_entry_t *f;
	if ((f = open_file_from_path((fat_fs_instance_t*)instance, path)) == NULL) {
		if (flags & O_CREAT) {
			fat_createfile((fat_fs_instance_t*)instance, path, 0); // XXX: set du vrai mode.
			if ((f = open_file_from_path((fat_fs_instance_t*)instance, path)) == NULL) {
				return NULL;
			}
		} else {
			return NULL;
		}
	}
	open_file_descriptor *ofd = kmalloc(sizeof(open_file_descriptor));
	size_t size_buffer = sizeof(ofd->buffer) < ((fat_fs_instance_t*)instance)->fat_info.BS.bytes_per_sector ? 
			sizeof(ofd->buffer) : ((fat_fs_instance_t*)instance)->fat_info.BS.bytes_per_sector;

	ofd->fs_instance = instance;
	ofd->first_cluster = f->cluster;
	ofd->file_size = f->size;
	ofd->current_cluster = ofd->first_cluster;
	ofd->current_octet = 0;
	ofd->current_octet_buf = 0;

	ofd->write = fat_write_file;
	ofd->read = fat_read_file;
	ofd->seek = fat_seek_file;
	ofd->close = fat_close;

	((fat_fs_instance_t*)instance)->fat_info.read_data(instance->device, ofd->buffer, size_buffer, ((fat_fs_instance_t*)instance)->fat_info.addr_data + (ofd->current_cluster - 2) * ((fat_fs_instance_t*)instance)->fat_info.BS.sectors_per_cluster * ((fat_fs_instance_t*)instance)->fat_info.BS.bytes_per_sector);

	kfree(f);

	return ofd;
}

int fat_close(open_file_descriptor *ofd) {
	if (ofd == NULL) {
		return -1;
	}
	kfree(ofd);
	return 0;
}

int fat_stat(fs_instance_t *instance, const char *path, struct stat *stbuf) {
	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));
	stbuf->st_mode = 0755;

	if(strcmp(path, "/") == 0) {
		stbuf->st_mode |= S_IFDIR;
	} else {
		directory_t *dir;
		char filename[256];
		char * pathdir = kmalloc(strlen(path) + 1);
		split_dir_filename(path, pathdir, filename);
		if ((dir = open_dir_from_path((fat_fs_instance_t*)instance, pathdir)) == NULL)
			return -ENOENT;
		kfree(pathdir);

		directory_entry_t *dir_entry = dir->entries;
		while (dir_entry) {
			if (strcmp(dir_entry->name, filename) == 0) {
					break;
			}
			dir_entry = dir_entry->next;
		}
		if (!dir_entry) {
			kfree(dir); // TODO: liberer memoire liste chainee.
			return -ENOENT;
		} else {
			if (dir_entry->attributes & 0x01) { // Read Only
				stbuf->st_mode &= ~0111;
			}
			if (dir_entry->attributes & 0x10) { // Dir.
				stbuf->st_mode |= S_IFDIR;
			} else {
				stbuf->st_mode |= S_IFREG;
			}
			stbuf->st_atime = dir_entry->access_time;
			stbuf->st_mtime = dir_entry->modification_time;
			stbuf->st_ctime = dir_entry->creation_time;
			stbuf->st_size = dir_entry->size;
		}
		kfree(dir); // TODO: liberer memoire liste chainee.
	}

	return res;
}

static int delete_dir_entry(fat_dir_entry_t *fdir, const char *name, int n) {
  char filename[256];
  int i;

  for (i = 0; i < n && fdir[i].utf8_short_name[0]; i++) {
    if ((unsigned char)fdir[i].utf8_short_name[0] != 0xE5) {
      if (fdir[i].file_attributes == 0x0F && ((lfn_entry_t*) &fdir[i])->seq_number & 0x40) {
			  int j;
			  uint8_t i_filename = 0;
			  uint8_t seq = ((lfn_entry_t*) &fdir[i])->seq_number - 0x40;
			  for (j = seq-1; j >= 0; j--) {
			    decode_long_file_name(filename + i_filename, (lfn_entry_t*) &fdir[i+j]);
			    i_filename += 13;
			  }

				if (strcmp(filename, name) == 0) {
					for (j = seq; j >= 0; j--) {
						fdir[i+j].utf8_short_name[0] = 0xE5;
					}
					return 0;
				}
        i += seq;
      } else {
        decode_short_file_name(filename, &fdir[i]);
				if (strcmp(filename, name) == 0) {
					fdir[i].utf8_short_name[0] = 0xE5;
					return 0;
				}
      }
    }
  }
	return 1;
}


static int delete_file_dir(fat_fs_instance_t *instance, int cluster, const char * name) {
  int n_dir_entries = instance->fat_info.BS.bytes_per_sector * instance->fat_info.BS.sectors_per_cluster / sizeof(fat_dir_entry_t);
	if (cluster >= 0) {

	  int n_clusters = 0;
	  int next = cluster;
	  while (!is_last_cluster(instance, next)) {
	    next = instance->fat_info.file_alloc_table[next];
	    n_clusters++;
	  }
	
	  fat_dir_entry_t * sub_dir = kmalloc(n_dir_entries * sizeof(fat_dir_entry_t) * n_clusters);
	
	  int c = 0;
	  next = cluster;
	  while (!is_last_cluster(instance, next)) {
	    instance->fat_info.read_data(instance->super.device, (uint8_t*)(sub_dir + c * n_dir_entries), n_dir_entries * sizeof(fat_dir_entry_t), instance->fat_info.addr_data + (next - 2) * instance->fat_info.BS.sectors_per_cluster * instance->fat_info.BS.bytes_per_sector);
	    next = instance->fat_info.file_alloc_table[next];
	    c++;
	  }
	
		if (delete_dir_entry(sub_dir, name, n_dir_entries * n_clusters) == 0) {
			c = 0;
			next = cluster;
		  while (!is_last_cluster(instance, next)) {
				instance->fat_info.write_data(instance->super.device, (uint8_t*)(sub_dir + c * n_dir_entries), n_dir_entries * sizeof(fat_dir_entry_t), instance->fat_info.addr_data + (next - 2) * instance->fat_info.BS.sectors_per_cluster * instance->fat_info.BS.bytes_per_sector);
		    next = instance->fat_info.file_alloc_table[next];
		    c++;
			}
			kfree(sub_dir);
			return 0;
		}
		kfree(sub_dir);
	} else {
    fat_dir_entry_t *root_dir = kmalloc(sizeof(fat_dir_entry_t) * instance->fat_info.BS.root_entry_count);
    instance->fat_info.read_data(instance->super.device, (uint8_t*)(root_dir), sizeof(fat_dir_entry_t) * instance->fat_info.BS.root_entry_count, instance->fat_info.addr_root_dir);
		if (delete_dir_entry(root_dir, name, n_dir_entries) == 0) {
			instance->fat_info.write_data(instance->super.device, (uint8_t*)(root_dir), sizeof(fat_dir_entry_t) * instance->fat_info.BS.root_entry_count, instance->fat_info.addr_root_dir);
			kfree(root_dir);
			return 0;
		}
		kfree(root_dir);
	}
	return 1;
}


int fat_unlink(fs_instance_t *instance, const char * path) {
  if (path[0] == '\0' || strcmp(path, "/") == 0)
    return -1;

  // Only absolute paths.
  if (path[0] != '/')
    return -1;

  char buf[256];
  int i = 1;
  while (path[i] == '/')
    i++;

  directory_t * dir = open_root_dir((fat_fs_instance_t*)instance);

  int j = 0;
  do {
    if (path[i] == '/' || path[i] == '\0') {
      buf[j] = '\0';

			int cluster = dir->cluster;
      if (j > 0 && open_next_dir((fat_fs_instance_t*)instance, dir, dir, buf) == 2) {
					return delete_file_dir((fat_fs_instance_t*)instance, cluster, buf);
			}

      j = 0;
    } else {
      buf[j] = path[i];
      j++;
    }
  } while (path[i++] != '\0');

	return 1;
}



void fat_init() {
	file_system_t *fs = kmalloc(sizeof(file_system_t));
	fs->name = "FAT";
	fs->mount = mount_FAT;
	fs->umount = umount_FAT;
	vfs_register_fs(fs);
}


