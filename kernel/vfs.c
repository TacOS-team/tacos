/**
 * @file vfs.c
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
 * Virtual File System. Gestion des points de montage.
 */

#include <fs/devfs.h>
#include <fs/fat.h>
#include <kdirent.h>
#include <kfcntl.h>
#include <kmalloc.h>
#include <vfs.h>
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
	int len;
	fs_instance_t *instance = get_instance_from_path(pathname, &len);
	if (instance && instance->open != NULL) {
		return instance->open(instance, pathname + len, flags);
	}
	if (instance == NULL && len == 0) {
		open_file_descriptor *ofd = kmalloc(sizeof(open_file_descriptor));
		
		ofd->readdir = vfs_readdir;
		ofd->close = vfs_close;
		
		return ofd;
	}
	return NULL;
}

int vfs_close(open_file_descriptor *ofd) {
	if (ofd == NULL) {
		return -1;
	}
	kfree(ofd);
	return 0;
}

void vfs_mount(const char *device, const char *mountpoint, const char *type) {
	available_fs_t *aux = fs_list;
	open_file_descriptor* ofd = NULL;
	while (aux != NULL) {
		if (strcmp(aux->fs->name, type) == 0) {
			mounted_fs_t *element = kmalloc(sizeof(mounted_fs_t));
			int len = strlen(mountpoint);
			element->name = kmalloc(len + 1);
			int i;
			for (i = 0; i <= len; i++)
				element->name[i] = mountpoint[i];
				
			if(device != NULL) {
				/* Open the mounted device */
				ofd = vfs_open(device, O_RDWR);
			}
			element->instance = aux->fs->mount(ofd);
			element->instance->device = ofd;
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
				/* Close the device ofd. */
				if (aux->instance->device != NULL && aux->instance->device->close) {
					aux->instance->device->close(aux->instance->device);
				}
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
			buf->st_blksize = 2048;
			return 0;
		}
	}
	return -ENOENT;
}

int vfs_unlink(const char *pathname) {
	int len;
	fs_instance_t *instance = get_instance_from_path(pathname, &len);
	if (instance) {
		if (instance->unlink != NULL) {
			if (pathname[len] == '\0') {
				return instance->unlink(instance, "/");
			} else {
				return instance->unlink(instance, pathname + len);
			}
		}
	} else {
		if (len == 0) {
			return 1;
		}
	}
	return -ENOENT;
}

int vfs_mknod(const char * pathname, mode_t mode, dev_t dev) {
	int len;
	fs_instance_t *instance = get_instance_from_path(pathname, &len);
	if (instance && instance->mknod != NULL) {
		return instance->mknod(instance, pathname + len, mode, dev);
	}
	return -1;

}

int vfs_mkdir(const char * pathname, mode_t mode) {
	int len;
	fs_instance_t *instance = get_instance_from_path(pathname, &len);
	if (instance && instance->mkdir != NULL) {
		return instance->mkdir(instance, pathname + len, mode);
	}
	return -1;
}

int vfs_readdir(open_file_descriptor * ofd, char * entries, size_t size) {
	size_t count = 0;
	size_t c = 0;
	mounted_fs_t *aux = mount_list;

	while (c < ofd->current_octet && aux) {
		aux = aux->next;
		c++;
	}

	while (aux != NULL && count < size) {
		struct dirent *d = (struct dirent *)(entries + count);
		d->d_ino = 1;
		d->d_reclen = sizeof(d->d_ino) + sizeof(d->d_reclen) + sizeof(d->d_type) + strlen(aux->name) + 1;
		//d.d_type = dir_entry->attributes;
		strcpy(d->d_name, aux->name);
		count += d->d_reclen;
		aux = aux->next;
		c++;
	}

	ofd->current_octet = c;

	return count;
}
