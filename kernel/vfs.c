/**
 * @file vfs.c
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
 * Virtual File System. Gestion des points de montage.
 */

#include <klog.h>
#include <kdriver.h>
#include <kfat.h>
#include <kmalloc.h>
#include <vfs.h>
#include <kdirent.h>
//XXX: libc
#include <string.h>

typedef struct _available_fs_t {
	file_system_t *fs;
	struct _available_fs_t *next;
} available_fs_t;

static available_fs_t *fs_list;

typedef struct _mounted_fs_t {
	fs_instance_t *instance;
	char *name;
	struct _mounted_fs_t *next;
} mounted_fs_t;

static mounted_fs_t *mount_list;

void vfs_register_fs(file_system_t *fs) {
	available_fs_t *element = kmalloc(sizeof(available_fs_t));
	element->fs = fs;
	element->next = fs_list;
	fs_list = element;
}

static fs_instance_t* get_instance_from_path(const char * pathname, int *len) {
	char *mountpoint;
	int i = 1;
	while (pathname[i] != '/' && pathname[i] != '\0') {
		i++;
	}
	if (i <= 1) {
		*len = 0;
		return NULL;
	}
	*len = i;
	mountpoint = kmalloc(i);

	int j;
	for (j = 0; j < i; j++) {
		mountpoint[j] = pathname[j+1];
	}
	mountpoint[i-1] = '\0';

	mounted_fs_t *aux = mount_list;
	while (aux != NULL) {
		if (strcmp(aux->name, mountpoint) == 0) {
			return aux->instance;
		}
		aux = aux->next;
	}
	return NULL;
}

open_file_descriptor * vfs_open(const char * pathname, uint32_t flags) {
	//XXX: Ceci devrait être dans un dev_fs !
	if(pathname[0] == '$')
	{
		// creation d un open_file_descriptor (XXX: passer dans le open du driver !)
		open_file_descriptor *ofd = kmalloc(sizeof(open_file_descriptor));
		ofd->flags = flags;
	
		driver_interfaces* di = find_driver(pathname+1);
		if(di != NULL) {
			if( di->open == NULL )
				kerr("No \"open\" method for device \"%s\".", pathname+1);
			else
				di->open(ofd);
				
			ofd->write = di->write;
			ofd->read = di->read;
			ofd->seek = di->seek;
			ofd->ioctl = di->ioctl;
			ofd->open = di->open;
			ofd->close = di->close;
			
			return ofd;
		} else {
			return NULL;
		}
	}

	int len;
	fs_instance_t *instance = get_instance_from_path(pathname, &len);
	if (instance->open != NULL) {
		return instance->open(instance, pathname + len, flags);
	}
	return NULL;
}

void vfs_mount(const char *device __attribute__((unused)), const char *mountpoint, const char *type) {
	available_fs_t *aux = fs_list;
	while (aux != NULL) {
		if (strcmp(aux->fs->name, type) == 0) {
			mounted_fs_t *element = kmalloc(sizeof(mounted_fs_t));
			int len = strlen(mountpoint);
			element->name = kmalloc(len + 1);
			int i;
			for (i = 0; i <= len; i++)
				element->name[i] = mountpoint[i];
			element->instance = aux->fs->mount(); //XXX: device en argument.
			element->next = mount_list;
			mount_list = element;
		}
		aux = aux->next;
	}
}

int vfs_umount(const char *mountpoint) {
	mounted_fs_t *aux = mount_list;
	while (aux != NULL) {
		if (strcmp(aux->name, mountpoint) == 0) {
			if (aux->instance->fs->umount != NULL) {
				aux->instance->fs->umount(aux->instance);
			}
			return 0;
		}
		aux = aux->next;
	}
	return 1;
}

int vfs_stat(const char *pathname, struct stat *buf) {
	int len;
	fs_instance_t *instance = get_instance_from_path(pathname, &len);
	if (instance) {
		if (instance->stat != NULL) {
			if (pathname[len] == '\0') {
				return instance->stat(instance, "/", buf);
			} else {
				return instance->stat(instance, pathname + len, buf);
			}
		}
	} else {
		if (len == 0) {
			buf->st_mode = S_IFDIR;
			buf->st_size = 0;
			return 0;
		}
	}
	return -ENOENT;
}

int vfs_mkdir(const char * pathname, mode_t mode) {
	int len;
	fs_instance_t *instance = get_instance_from_path(pathname, &len);
	if (instance && instance->mkdir != NULL) {
		return instance->mkdir(instance, pathname + len, mode);
	}
	return -1;
}

int vfs_opendir(const char *pathname) {
	int len;
	fs_instance_t *instance = get_instance_from_path(pathname, &len);
	if (instance) {
		if (instance->opendir != NULL) {
			if (pathname[len] == '\0') {
				return instance->opendir(instance, "/");
			} else {
				return instance->opendir(instance, pathname + len);
			}
		}
	} else {
		if (len == 0) {
			return 0;
		}
	}
	return -ENOENT;
}

int vfs_readdir(const char * pathname, int iter, char * filename) {
	int len;
	fs_instance_t *instance = get_instance_from_path(pathname, &len);
	if (instance) {
		if (instance->readdir != NULL) {
			if (pathname[len] == '\0') {
				return instance->readdir(instance, "/", iter, filename);
			} else {
				return instance->readdir(instance, pathname + len, iter, filename);
			}
		}
	} else {
		if (len == 0) {
			mounted_fs_t *aux = mount_list;
			while (aux != NULL) {
				if (iter == 0) {
					strcpy(filename, aux->name);
					return 0;
				}
				aux = aux->next;
				iter--;
			}
		}
	}
	return -1;
}
