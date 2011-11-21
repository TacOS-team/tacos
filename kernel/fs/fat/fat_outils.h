#ifndef _FAT_OUTILS_H
#define _FAT_OUTILS_H

#include "fat_internal.h"

int fat_last_cluster(fat_fs_instance_t *instance);
int fat_is_free_cluster(int cluster);
int fat_is_last_cluster(fat_fs_instance_t *instance, int cluster);
int fat_is_used_cluster(fat_fs_instance_t *instance, int cluster);
void fat_encode_long_file_name(char * name, lfn_entry_t * long_file_name, int n_entries);
char * fat_decode_long_file_name(char * name, lfn_entry_t * long_file_name);
time_t convert_datetime_fat_to_time_t(fat_date_t *date, fat_time_t *time);
void convert_time_t_to_datetime_fat(time_t time, fat_time_t *timefat, fat_date_t *datefat);
void fat_dir_entry_to_directory_entry(char *filename, fat_dir_entry_t *dir, directory_entry_t *entry);
char * fat_lfn_to_sfn(char * filename);
void fat_decode_short_file_name(char *filename, fat_dir_entry_t *fdir);
directory_entry_t * fat_decode_lfn_entry(lfn_entry_t* fdir);
directory_entry_t * fat_decode_sfn_entry(fat_dir_entry_t *fdir);
void fat_split_dir_filename(const char * path, char * dir, char * filename);

#endif
