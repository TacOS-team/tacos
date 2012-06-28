/**
 * @file devfs.c
 *
 * @author TacOS developers 
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
 * @brief DevFS
 */

#include <fs/devfs.h>
#include <kdirent.h>
#include <klibc/string.h>
#include <klog.h>
#include <kmalloc.h>
#include <kerrno.h>
#include <types.h>
#include <vfs.h>

#define MAX_DRIVERS 64 /**< Nombre maximum de drivers. */

static int devfs_readdir(open_file_descriptor * ofd, char * entries, size_t size);

static dentry_t root_devfs;
static struct _open_file_operations_t devfs_fops = {.write = NULL, .read = NULL, .seek = NULL, .ioctl = NULL, .open = NULL, .close = NULL, .readdir = devfs_readdir};

typedef struct {
	char used;
	char* name;
	device_type_t type;
	void* di;
} driver_entry;

static driver_entry driver_list[MAX_DRIVERS];

/** 
 * @brief Initialise la liste des drivers.
 * 	Initialise la liste des drivers.
 *
 */
void init_driver_list()
{
	int i;
	for(i=0; i<MAX_DRIVERS; i++)
	{
			driver_list[i].used = 0;
			driver_list[i].name = NULL;
			driver_list[i].type = CHARDEV;
			driver_list[i].di = NULL;
	}
}

/** 
 * @brief Trouve les interfaces d'un driver en fonction de son nom.
 * 
 *	Cherche dans la liste des drivers celui qui s'appelle name, et retourne ses interfaces
 *
 * @param name chaine identifiant le driver
 * 
 * @return interfaces du driver trouvé
 */
driver_entry* find_driver(const char* name)
{
	int i = 0;
	driver_entry* ret = NULL;
	
	while(i<MAX_DRIVERS && ret == NULL)
	{
		if(driver_list[i].used)
		{
			if(strcmp(driver_list[i].name, name) == 0)
				ret = &driver_list[i];
		}
		i++;
	}
	
	return ret;
}

/** 
 * @brief Enregistre le chardev dans la liste.
 * 
 *	Enregistre le driver dans la liste.
 *
 * @param name chaine identifiant le driver
 * @param di structure contenant les interfaces du driver
 * 
 * @return -1 en cas d'erreur, 0 sinon
 */
int register_chardev(const char* name, chardev_interfaces* di)
{
	int i = 0;
	int done = 0;
	while(i<MAX_DRIVERS && !done)
	{
		if(!driver_list[i].used)
		{
			driver_list[i].used = 1;
			driver_list[i].name = (char*)name;
			driver_list[i].di = di;
			driver_list[i].type = CHARDEV;
			done = 1;
		}
		
		i++;
	}
	return done-1; /* Retourne -1 en cas d'erreur, 0 sinon */
}

/** 
 * @brief Enregistre le blkdev dans la liste.
 * 
 *	Enregistre le driver dans la liste.
 *
 * @param name chaine identifiant le driver
 * @param di structure contenant les interfaces du driver
 * 
 * @return -1 en cas d'erreur, 0 sinon
 */
int register_blkdev(const char* name, blkdev_interfaces* di)
{
	int i = 0;
	int done = 0;
	while(i<MAX_DRIVERS && !done)
	{
		if(!driver_list[i].used)
		{
			driver_list[i].used = 1;
			driver_list[i].name = (char*)name;
			driver_list[i].di = di;
			driver_list[i].type = BLKDEV;
			done = 1;
		}
		
		i++;
	}
	return done-1; /* Retourne -1 en cas d'erreur, 0 sinon */
}

static int devfs_readdir(open_file_descriptor * ofd, char * entries, size_t size) {
	size_t count = 0;

	int i = ofd->current_octet;
	while (i < MAX_DRIVERS && count < size) {
		if (driver_list[i].used) {
			struct dirent *d = (struct dirent *)(entries + count);
			d->d_ino = 1;
			d->d_reclen = sizeof(d->d_ino) + sizeof(d->d_reclen) + sizeof(d->d_type) + strlen(driver_list[i].name) + 1;
			//d.d_type = dir_entry->attributes;
			strcpy(d->d_name, driver_list[i].name);
			count += d->d_reclen;
		}
		i++;
	}
	ofd->current_octet = i;

	return count;
}

static dentry_t *devfs_getroot() {
	return &root_devfs;
}

static dentry_t* devfs_lookup(struct _fs_instance_t *instance, struct _dentry_t* dentry __attribute__((unused)), const char * name) {
	//TODO: gérer dentry pour avoir des sous-dossiers.

	driver_entry *drentry = find_driver(name);
	if (drentry != NULL) {
		inode_t *inode = kmalloc(sizeof(inode_t));
		inode->i_ino = 42; //XXX
		inode->i_mode = 0755;
		inode->i_uid = 0;
		inode->i_gid = 0;
		inode->i_size = 0;
		inode->i_atime = inode->i_ctime = inode->i_mtime = inode->i_dtime = 0; // XXX
		inode->i_nlink = 1;
		inode->i_blocks = 1;
		inode->i_instance = instance;
		inode->i_fops = kmalloc(sizeof(open_file_operations_t));
		inode->i_fs_specific = (chardev_interfaces*)(drentry->di);

		switch(drentry->type) {
			case CHARDEV:
				inode->i_fops->write = ((chardev_interfaces*)(drentry->di))->write;
				inode->i_fops->read = ((chardev_interfaces*)(drentry->di))->read; 
				inode->i_fops->seek = NULL; /* à implémenter */
				inode->i_fops->close = ((chardev_interfaces*)(drentry->di))->close;
				inode->i_fops->ioctl = ((chardev_interfaces*)(drentry->di))->ioctl;
				
				/*if(((chardev_interfaces*)(drentry->di))->open != NULL) {
					((chardev_interfaces*)(drentry->di))->open(ofd);
				}
				else {
					kerr("No \"open\" method for %s.", drentry->name);
				}*/
				
				break;
			case BLKDEV:
				inode->i_fops->write = NULL; /* à implémenter */
				inode->i_fops->read = NULL; /* à implémenter */
				inode->i_fops->seek = NULL; /* à implémenter */
				inode->i_fops->close = ((blkdev_interfaces*)(drentry->di))->close;
				inode->i_fops->ioctl = NULL; /* à implémenter */
				
				/*
				if(((blkdev_interfaces*)(drentry->di))->open != NULL) {
					((blkdev_interfaces*)(drentry->di))->open(ofd);
				}
				else {
					kerr("No \"open\" method for %s.", drentry->name);
				}
				*/
				break;
		}
		dentry_t *d = kmalloc(sizeof(dentry_t));
		d->d_name = kmalloc(strlen(name) + 1);
		strcpy(d->d_name, name);
		d->d_inode = inode;
		return d;
	}
	return NULL;
}

static int devfs_stat(fs_instance_t *instance __attribute__ ((unused)), const char *path, struct stat *stbuf) {
	int res = 0;
	unsigned int i,j;
	char buf[64];

	memset(stbuf, 0, sizeof(struct stat));
	stbuf->st_mode = 0755;
	stbuf->st_size = 10;
	stbuf->st_atime = 0;
	stbuf->st_mtime = 0;
	stbuf->st_ctime = 0;
	stbuf->st_blksize = 2048;

	if(strcmp(path, "/") == 0) {
		stbuf->st_mode |= S_IFDIR;
		return 0;
	} else {
		i=0;
		while (path[i] == '/') 
			i++;
		j=0;
		while(path[i] != '\0' && path[i] != '/') {
			buf[j] = path[i];
			i++;
			j++;
		}
		buf[j] = '\0';
		
		if(find_driver(buf) != NULL) {
			stbuf->st_mode |= S_IFCHR;
		} else {
			return -ENOENT;
		}
	}
	return res;
}

fs_instance_t* mount_devfs() {
//	klog("mounting DevFS");

	fs_instance_t *instance = kmalloc(sizeof(fs_instance_t));
	instance->mkdir = NULL;
	instance->stat = devfs_stat;
	instance->getroot = devfs_getroot;
	instance->lookup = devfs_lookup;
	
	return instance;
}

void umount_devfs(fs_instance_t *instance) {
	kfree(instance);
}

void devfs_init() {
	file_system_t *fs = kmalloc(sizeof(file_system_t));
	root_devfs.d_name = "";
	root_devfs.d_inode = kmalloc(sizeof(inode_t));
	root_devfs.d_inode->i_ino = 0;
	root_devfs.d_inode->i_mode = S_IFDIR | 00755;
	root_devfs.d_inode->i_fops = &devfs_fops;
	init_driver_list();
	
	fs->name = "DevFS";
	fs->mount = mount_devfs;
	fs->umount = umount_devfs;
	vfs_register_fs(fs);
}
