/**
 * @file fat_outils.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012 - TacOS developers.
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

#include "fat_outils.h"
#include <clock.h>
#include <string.h>
#include <kmalloc.h>
#include <ctype.h>

int fat_last_cluster(fat_fs_instance_t *instance) {
	if (instance->fat_info.fat_type == FAT12) {
		return 0xFFF;
	} else if (instance->fat_info.fat_type == FAT16) {
		return 0xFFFF;
	} else {
		return 0x0FFFFFFF;
	}
}

int fat_is_free_cluster(int cluster) {
	return cluster == 0;
}

int fat_is_last_cluster(fat_fs_instance_t *instance, int cluster) {
	if (instance->fat_info.fat_type == FAT12) {
		return cluster >= 0xFF8 && cluster <= 0xFFF;
	} else if (instance->fat_info.fat_type == FAT16) {
		return cluster >= 0xFFF8 && cluster <= 0xFFFF;
	} else {
		return cluster >= 0x0FFFFFF8 && cluster <= 0x0FFFFFFF;
	}
}

int fat_is_used_cluster(fat_fs_instance_t *instance, int cluster) {
	if (instance->fat_info.fat_type == FAT12) {
		return cluster >= 0x002 && cluster <= 0xFEF;
	} else if (instance->fat_info.fat_type == FAT16) {
		return cluster >= 0x0002 && cluster <= 0xFFEF;
	} else {
		return cluster >= 0x00000002 && cluster <= 0x0FFFFFEF;
	}
}

void fat_encode_long_file_name(char * name, lfn_entry_t * long_file_name, int n_entries) {
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

char * fat_decode_long_file_name(char * name, lfn_entry_t * long_file_name) {

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

time_t convert_datetime_fat_to_time_t(fat_date_t *date, fat_time_t *time) {
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

void convert_time_t_to_datetime_fat(time_t time, fat_time_t *timefat, fat_date_t *datefat) {
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

void fat_dir_entry_to_directory_entry(char *filename, fat_dir_entry_t *dir, directory_entry_t *entry) {
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

char * fat_lfn_to_sfn(char * filename) {
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

void fat_decode_short_file_name(char *filename, fat_dir_entry_t *fdir) {
  int j, k;
	int notspace = 0;

  // Copy basis name.
  for (j = 7; j >= 0; j--) {
		if (notspace || fdir->utf8_short_name[j] != ' ') {
			if (!notspace) {
				notspace = j;
			}
			if (fdir->reserved & 0x08) {
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
			if (fdir->reserved & 0x10) {
				filename[notspace + k] = tolower(fdir->file_extension[k]);
			} else {
				filename[notspace + k] = fdir->file_extension[k];
			}
		}
  }

	filename[notspace + notspaceext + 1] = '\0';

}

directory_entry_t * fat_decode_lfn_entry(lfn_entry_t* fdir) {
	int j;
	char filename[256];
	uint8_t i_filename = 0;
	uint8_t seq = fdir->seq_number - 0x40;
	for (j = seq-1; j >= 0; j--) {
		fat_decode_long_file_name(filename + i_filename, &fdir[j]);
		i_filename += 13;
	}
	directory_entry_t *dir_entry = kmalloc(sizeof(directory_entry_t));
	fat_dir_entry_to_directory_entry(filename, (fat_dir_entry_t*)&fdir[seq], dir_entry);
	return dir_entry;
}

directory_entry_t * fat_decode_sfn_entry(fat_dir_entry_t *fdir) {
  char filename[256];
	fat_decode_short_file_name(filename, fdir);
	directory_entry_t *dir_entry = kmalloc(sizeof(directory_entry_t));
	fat_dir_entry_to_directory_entry(filename, fdir, dir_entry);
	return dir_entry;
}


void fat_split_dir_filename(const char * path, char * dir, char * filename) {
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

