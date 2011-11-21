/**
 * @file fat_internal.c
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

#ifndef EOF
# define EOF (-1)
#endif

#include "fat_outils.h"

/*
 * Read the File Allocation Table.
 */
void read_fat(fat_fs_instance_t *instance) {
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

static void read_dir_entries(fat_dir_entry_t *fdir, directory_t *dir, int n) {
	int i;
	for (i = 0; i < n && fdir[i].utf8_short_name[0]; i++) {
		directory_entry_t * dir_entry;

		if ((unsigned char)fdir[i].utf8_short_name[0] != 0xE5) {
			if (fdir[i].file_attributes == 0x0F && ((lfn_entry_t*) &fdir[i])->seq_number & 0x40) {
				dir_entry = fat_decode_lfn_entry((lfn_entry_t*) &fdir[i]);
				uint8_t seq = ((lfn_entry_t*) &fdir[i])->seq_number - 0x40;
				i += seq;
			} else {
				dir_entry = fat_decode_sfn_entry(&fdir[i]);
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
	while (!fat_is_last_cluster(instance, next)) {
		next = instance->fat_info.file_alloc_table[next];
		n_clusters++;
	}

	int n_dir_entries = instance->fat_info.BS.bytes_per_sector * instance->fat_info.BS.sectors_per_cluster / sizeof(fat_dir_entry_t);
	fat_dir_entry_t * sub_dir = kmalloc(n_dir_entries * sizeof(fat_dir_entry_t) * n_clusters);

	int c = 0;
	next = cluster;
	while (!fat_is_last_cluster(instance, next)) {
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
	fat_split_dir_filename(path, dir, filename);

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
    return fat_last_cluster(instance);
  }
  int next = alloc_cluster(instance, n - 1);
  unsigned int i;
  for (i = 0; i < instance->fat_info.total_data_clusters; i++) {
    if (fat_is_free_cluster(instance->fat_info.file_alloc_table[i])) {
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
    while (!fat_is_last_cluster(instance, next)) {
      next = instance->fat_info.file_alloc_table[next];
      n_clusters++;
    }
  
    int n_dir_entries = instance->fat_info.BS.bytes_per_sector * instance->fat_info.BS.sectors_per_cluster / sizeof(fat_dir_entry_t);
    fat_dir_entry_t * dir_entries = kmalloc(n_dir_entries * sizeof(fat_dir_entry_t) * n_clusters);
  
    int c = 0;
    next = dir->cluster;
    while (!fat_is_last_cluster(instance, next)) {
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
      while (!fat_is_last_cluster(instance, instance->fat_info.file_alloc_table[next])) {
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

static void load_buffer(open_file_descriptor *ofd) {
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
			    fat_decode_long_file_name(filename + i_filename, (lfn_entry_t*) &fdir[i+j]);
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
        fat_decode_short_file_name(filename, &fdir[i]);
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
	  while (!fat_is_last_cluster(instance, next)) {
	    next = instance->fat_info.file_alloc_table[next];
	    n_clusters++;
	  }
	
	  fat_dir_entry_t * sub_dir = kmalloc(n_dir_entries * sizeof(fat_dir_entry_t) * n_clusters);
	
	  int c = 0;
	  next = cluster;
	  while (!fat_is_last_cluster(instance, next)) {
	    instance->fat_info.read_data(instance->super.device, (uint8_t*)(sub_dir + c * n_dir_entries), n_dir_entries * sizeof(fat_dir_entry_t), instance->fat_info.addr_data + (next - 2) * instance->fat_info.BS.sectors_per_cluster * instance->fat_info.BS.bytes_per_sector);
	    next = instance->fat_info.file_alloc_table[next];
	    c++;
	  }
	
		if (delete_dir_entry(sub_dir, name, n_dir_entries * n_clusters) == 0) {
			c = 0;
			next = cluster;
		  while (!fat_is_last_cluster(instance, next)) {
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


