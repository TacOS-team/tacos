/**
 * @file devfs.c
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
 * DevFS
 */

#include <fs/devfs.h>
#include <kdirent.h>
#include <klog.h>
#include <kmalloc.h>
#include <types.h>
#include <vfs.h>

#include <string.h>

#define MAX_DRIVERS 64

typedef struct {
	char used;
	char* name;
	device_type_t type;
	void* di;
}driver_entry;

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

int devfs_opendir(fs_instance_t *instance __attribute__((unused)), const char * path) {
	if(strcmp(path,"/") != 0) {
		return -ENOENT;
	}		
	return 0;
}

int devfs_readdir(fs_instance_t *instance __attribute__((unused)), const char * path, int iter, char * filename) {
	int i;
	
	if (path[0] != '/')
		return -ENOENT;
	else {
		i=0;
		while (path[i] == '/') 
			i++;
			
		if(path[i] == '\0') {
			i=0;
			while(i<MAX_DRIVERS) {
				while(i<MAX_DRIVERS && driver_list[i].used == 0)
					i++;
				if(!iter && driver_list[i].used == 1) {
					strcpy(filename, driver_list[i].name);
					return 0;
				}
				iter--;
				i++;
			}
		}
	}
	return 1;
}

open_file_descriptor* devfs_open_file(fs_instance_t *instance, const char * path, uint32_t flags) {
	unsigned int i,j;
	char buf[64];
	driver_entry* drentry;
	
	if (path[0] != '/')
		return NULL;
	else {
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
		drentry = find_driver(buf);
		if(drentry != NULL) {
			
			open_file_descriptor *ofd = kmalloc(sizeof(open_file_descriptor));
			
			ofd->flags = flags;
			ofd->fs_instance = instance;
			ofd->first_cluster = 0;
			ofd->file_size = 512; /* TODO */
			ofd->current_cluster = 0;
			ofd->current_octet = 0;
			ofd->current_octet_buf = 0;
			ofd->extra_data = drentry->di; /* XXX Un peu laid mais ça fera la blague pour le moment */
			
			switch(drentry->type) {
				case CHARDEV:
					ofd->write = ((chardev_interfaces*)(drentry->di))->write;
					ofd->read = ((chardev_interfaces*)(drentry->di))->read; 
					ofd->seek = NULL; /* à implémenter */
					ofd->close = ((chardev_interfaces*)(drentry->di))->close;
					ofd->ioctl = ((chardev_interfaces*)(drentry->di))->ioctl;
					
					if(((chardev_interfaces*)(drentry->di))->open != NULL) {
						((chardev_interfaces*)(drentry->di))->open(ofd);
					}
					else {
						kerr("No \"open\" method for %s.", drentry->name);
					}
					
					break;
				case BLKDEV:
					ofd->write = NULL; /* à implémenter */
					ofd->read = NULL; /* à implémenter */
					ofd->seek = NULL; /* à implémenter */
					ofd->close = ((blkdev_interfaces*)(drentry->di))->close;
					ofd->ioctl = NULL; /* à implémenter */
					
					if(((blkdev_interfaces*)(drentry->di))->open != NULL) {
						((blkdev_interfaces*)(drentry->di))->open(ofd);
					}
					else {
						kerr("No \"open\" method for %s.", drentry->name);
					}
					break;
				default:
					ofd = NULL;
			}
			return ofd;
		}
	}
	return NULL;
}

int devfs_stat(fs_instance_t *instance __attribute__ ((unused)), const char *path, struct stat *stbuf) {
	int res = 0;
	unsigned int i,j;
	char buf[64];

	memset(stbuf, 0, sizeof(struct stat));
	stbuf->st_mode = 0755;
	stbuf->st_size = 10;
	stbuf->st_atime = 0;
	stbuf->st_mtime = 0;
	stbuf->st_ctime = 0;

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
	instance->open = devfs_open_file;
	instance->mkdir = NULL;
	instance->readdir = devfs_readdir;
	instance->opendir = devfs_opendir;
	instance->stat = devfs_stat;
	
	return instance;
}

void umount_devfs(fs_instance_t *instance) {
	kfree(instance);
}

void devfs_init() {
	file_system_t *fs = kmalloc(sizeof(file_system_t));
	
	init_driver_list();
	
	fs->name = "DevFS";
	fs->mount = mount_devfs;
	fs->umount = umount_devfs;
	vfs_register_fs(fs);
}
