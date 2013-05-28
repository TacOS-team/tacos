/**
 * @file fat_functions.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012, 2013 - TacOS developers.
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
 * @brief Fonctions pour le driver FAT.
 */

#include <fs/fat.h>
#include <kdirent.h>
#include <kfcntl.h>
#include <kstat.h>
#include <klibc/string.h>
#include <klog.h>
#include <kmalloc.h>
#include <kerrno.h>
#include <types.h>

#include "fat_outils.h"

#include "fat_internal.c"

dentry_t *fat_getroot(struct _fs_instance_t *instance) {
	return ((fat_fs_instance_t*)instance)->root;
}


dentry_t* fat_lookup(struct _fs_instance_t *instance, struct _dentry_t* dentry, const char * name) {
	fat_direntry_t *d = kmalloc(sizeof(fat_direntry_t));

	// Get prev_dir.
	fat_direntry_t *prev_dentry = (fat_direntry_t*) dentry;
	if (prev_dentry->fat_directory == NULL) {
		prev_dentry->fat_directory = kmalloc(sizeof(directory_t));
		open_dir((fat_fs_instance_t*)instance, prev_dentry->fat_entry->cluster, prev_dentry->fat_directory);
	}

	// Get directory_entry that match this name.
	directory_entry_t *next_dentry = open_next_direntry(prev_dentry->fat_directory, name);

	if (!next_dentry) {
		return NULL;
	}

	d->fat_directory = NULL;
	d->fat_entry = next_dentry;
	d->super.d_inode = kmalloc(sizeof(inode_t));
	d->super.d_pdentry = dentry;
	d->super.d_inode->i_fops = &fatfs_fops;
	d->super.d_inode->i_ino = 0;
	d->super.d_inode->i_mode = 00755;
	d->super.d_inode->i_mode |= (next_dentry->attributes & 0x10) ? S_IFDIR : S_IFREG;
	d->super.d_inode->i_uid = 0;
	d->super.d_inode->i_gid = 0;
	d->super.d_inode->i_size = next_dentry->size;
	d->super.d_inode->i_atime = next_dentry->access_time;
	d->super.d_inode->i_mtime = next_dentry->modification_time;
	d->super.d_inode->i_ctime = next_dentry->creation_time;
	d->super.d_inode->i_nlink = 1;
	d->super.d_inode->i_flags = 0; //XXX
	d->super.d_inode->i_blocks = 1; //XXX
	d->super.d_inode->i_instance = NULL; //XXX
	fat_extra_data_t *fs_specific = kmalloc(sizeof(fat_extra_data_t));
	fs_specific->first_cluster = next_dentry->cluster;
	fs_specific->current_cluster = next_dentry->cluster;
	fs_specific->current_octet_buf = 0;
	d->super.d_inode->i_fs_specific = fs_specific;

	return (dentry_t*)d;
}

int fat_readdir(open_file_descriptor * ofd, char * entries, size_t size) {
	fat_fs_instance_t *instance = (fat_fs_instance_t*) ofd->fs_instance;
	size_t count = 0;
	size_t c = 0;

	directory_t *dir;
	if (ofd->first_cluster > 0) {
		dir = kmalloc(sizeof(directory_t));
		open_dir(instance, ofd->first_cluster, dir);
	} else {
		dir = open_root_dir(instance);
	}
	directory_entry_t *dir_entry = dir->entries;

	kfree(dir);

	while (c < ofd->current_octet && dir_entry) {
		dir_entry = dir_entry->next;
		c++;
	}

	while (dir_entry && count <= size) {
		struct dirent *d = (struct dirent *)(entries + count);
		d->d_ino = 1;
		d->d_reclen = sizeof(d->d_ino) + sizeof(d->d_reclen) + sizeof(d->d_type) + strlen(dir_entry->name) + 1;
		//d.d_type = dir_entry->attributes;
		strcpy(d->d_name, dir_entry->name);
		count += d->d_reclen;
		dir_entry = dir_entry->next;
		c++;
	}

	ofd->current_octet = c;

	return count;
}

// TODO: Est-ce que offset peut être négatif ? Si oui, le gérer.
int fat_seek_file(open_file_descriptor * ofd, long offset, int whence) {
	fat_fs_instance_t *instance = (fat_fs_instance_t*) ofd->fs_instance;
	fat_extra_data_t *extra_data = (fat_extra_data_t*) ofd->extra_data;
	switch (whence) {
	case SEEK_SET: // depuis le debut du fichier
		if ((unsigned long) offset < ofd->file_size) {
			// On se met au début :
			extra_data->current_cluster = extra_data->first_cluster;
			ofd->current_octet = 0;

			// On avance de cluster en cluster.
			while (offset >= (long)instance->fat_info.bytes_per_cluster) {
				offset -= instance->fat_info.bytes_per_cluster;
				ofd->current_octet += instance->fat_info.bytes_per_cluster;
				extra_data->current_cluster = instance->fat_info.file_alloc_table[extra_data->current_cluster];
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
				extra_data->current_cluster = instance->fat_info.file_alloc_table[extra_data->current_cluster];
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

ssize_t fat_read_file(open_file_descriptor * ofd, void * buf, size_t count) {
	fat_fs_instance_t *instance = (fat_fs_instance_t*) ofd->fs_instance;
	fat_extra_data_t *extra_data = (fat_extra_data_t*) ofd->extra_data;
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
			char c = extra_data->buffer[extra_data->current_octet_buf];
			ret++;
			extra_data->current_octet_buf++;
			ofd->current_octet++;
			((char*) buf)[j++] = c;
			int current_octet_cluster = ofd->current_octet % instance->fat_info.bytes_per_cluster;
			if (current_octet_cluster == 0) {
				extra_data->current_cluster = instance->fat_info.file_alloc_table[extra_data->current_cluster];
				load_buffer(ofd);
			} else if (extra_data->current_octet_buf >= size_buffer) {
				load_buffer(ofd);
			}
		}
	}
	
	return ret;
}

int fat_mkdir (fs_instance_t *instance, const char * path, mode_t mode __attribute__((__unused__))) {
  char * dir = kmalloc(strlen(path));
  char filename[256];
  fat_split_dir_filename(path, dir, filename);

  char * sfn = fat_lfn_to_sfn(filename);

  int n_entries = 1 + ((strlen(filename) - 1) / 13);
  lfn_entry_t * long_file_name = kmalloc(sizeof(lfn_entry_t) * (n_entries + 1));
  fat_dir_entry_t *fentry = (fat_dir_entry_t*) &long_file_name[n_entries];

  fat_encode_long_file_name(filename, long_file_name, n_entries);

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

/*
int fat_createfile (fat_fs_instance_t* instance, const char * path, mode_t mode __attribute__((__unused__))) {
  char * dir = kmalloc(strlen(path));
  char filename[256];
  fat_split_dir_filename(path, dir, filename);

  char * sfn = fat_lfn_to_sfn(filename);

  int n_entries = 1 + ((strlen(filename) - 1) / 13);
  lfn_entry_t * long_file_name = kmalloc(sizeof(lfn_entry_t) * (n_entries + 1));
  fat_dir_entry_t *fentry = (fat_dir_entry_t*) &long_file_name[n_entries];

  fat_encode_long_file_name(filename, long_file_name, n_entries);

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
} */

int fat_open(open_file_descriptor *ofd) {
	load_buffer(ofd);
	return 0;
}

int fat_close(open_file_descriptor *ofd) {
	if (ofd == NULL) {
		return -1;
	}
	kfree(ofd);
	return 0;
}

/*
int fat_stat(inode_t *inode, struct stat *stbuf) {
	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));
	stbuf->st_mode = 0755;

	if(strcmp(path, "/") == 0) {
		stbuf->st_mode |= S_IFDIR;
		stbuf->st_blksize = ((fat_fs_instance_t*)instance)->fat_info.bytes_per_cluster;
	} else {
		directory_t *dir;
		char filename[256];
		char * pathdir = kmalloc(strlen(path) + 1);
		fat_split_dir_filename(path, pathdir, filename);
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
			stbuf->st_blksize = ((fat_fs_instance_t*)instance)->fat_info.bytes_per_cluster;
		}
		kfree(dir); // TODO: liberer memoire liste chainee.
	}

	return res;
}
*/

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


