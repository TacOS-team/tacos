/**
 * @file procfs.c
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
 * Description de ce que fait le fichier
 */

#include <fs/procfs.h>
#include <kdirent.h>
#include <klibc/string.h>
#include <klog.h>
#include <kmalloc.h>
#include <kprocess.h>
#include <kerrno.h>
#include <types.h>
#include <vfs.h>
#include <klibc/stdlib.h>

static fs_instance_t* mount_procfs();
static void umount_procfs(fs_instance_t *instance);

static file_system_t proc_fs = {.name="ProcFS", .unique_inode=0, .mount=mount_procfs, .umount=umount_procfs};

static int procfs_readdir(open_file_descriptor * ofd, char * entries, size_t size);

static dentry_t root_procfs;
static struct _open_file_operations_t procfs_fops = {.write = NULL, .read = NULL, .seek = NULL, .ioctl = NULL, .open = NULL, .close = NULL, .readdir = procfs_readdir};

typedef struct {
	int pid;
	process_t *process;
	// TODO.
} extra_data_procfs_t;

typedef struct {
	char* filename;
	size_t (*read)(open_file_descriptor *, void*, size_t);
} procfs_file_t;

#ifndef EOF
# define EOF (-1)
#endif

char* proc_status[]= {"IDLE", "RUNNING", "WAITING", "SUSPEND", "TERMINATED"};

static dentry_t *procfs_getroot() {
	return &root_procfs;
}

static size_t procfs_read_name(open_file_descriptor* ofd, void* buffer, size_t count __attribute__((unused))) {
	extra_data_procfs_t *extra = ofd->extra_data;
	process_t* process = extra->process;
	if(ofd->current_octet == 0) {
		strcpy(buffer,process->name);
		ofd->current_octet = strlen(process->name);
		return ofd->current_octet;
	}
	else
		return EOF;
}

static size_t procfs_read_ppid(open_file_descriptor* ofd, void* buffer, size_t count __attribute__((unused))) {
	extra_data_procfs_t *extra = ofd->extra_data;
	process_t* process = extra->process;
	if(ofd->current_octet == 0) {
		itoa(buffer, 10, process->ppid);
		ofd->current_octet = strlen(buffer);
		return ofd->current_octet;
	}
	else
		return EOF;
}

static size_t procfs_read_state(open_file_descriptor* ofd, void* buffer, size_t count __attribute__((unused))) {
	extra_data_procfs_t *extra = ofd->extra_data;
	process_t* process = extra->process;
	if(ofd->current_octet == 0) {
		strcpy(buffer,proc_status[process->state]);
		ofd->current_octet = strlen(buffer);
		return ofd->current_octet;
	}
	else
		return EOF;
}

procfs_file_t procfs_file_list[] = {{"cmd_line", procfs_read_name},
									{"ppid", procfs_read_ppid},
									{"state", procfs_read_state}};
									

int procfs_close(open_file_descriptor *ofd) {
	return kfree(ofd);
}

static int procfs_readdir(open_file_descriptor * ofd, char * entries, size_t size) {
	size_t count = 0;
	unsigned int i = ofd->current_octet;

	extra_data_procfs_t *extra = ofd->extra_data;

	if (extra->pid == -1) {
		while (i < MAX_PROC && count < size) {
			if (get_process_array(i) != NULL) {
				struct dirent *d = (struct dirent *)(entries + count);
				d->d_ino = 1;
				itoa (d->d_name, 10, get_process_array(i)->pid);
				d->d_reclen = sizeof(d->d_ino) + sizeof(d->d_reclen) + sizeof(d->d_type) + strlen(d->d_name) + 1;
				//d.d_type = dir_entry->attributes;
				count += d->d_reclen;
			}
			i++;
		}
	} else if (extra->pid >= 0) {
		int pid = extra->pid;
		if (find_process(pid) != NULL) {
			while (i < sizeof(procfs_file_list)/sizeof(procfs_file_t) && count < size) {
				struct dirent *d = (struct dirent *)(entries + count);
				d->d_ino = 1;
				strcpy(d->d_name, procfs_file_list[i].filename);
				d->d_reclen = sizeof(d->d_ino) + sizeof(d->d_reclen) + sizeof(d->d_type) + strlen(d->d_name) + 1;
				count += d->d_reclen;
				i++;
			}
		}
	}
	ofd->current_octet = i;

	return count;
}


static dentry_t* procfs_lookup(struct _fs_instance_t *instance, struct _dentry_t* dentry, const char * name) {
	int pid;
	process_t* process = NULL;
	
	if (dentry == &root_procfs) {
		pid = atoi(name);
		process = find_process(pid);
		if(process != NULL) {
			inode_t *inode = kmalloc(sizeof(inode_t));
			inode->i_ino = pid + 1;
			inode->i_mode = 0755 | S_IFDIR;
			inode->i_uid = 0;
			inode->i_gid = 0;
			inode->i_size = 512;
			inode->i_atime = inode->i_ctime = inode->i_mtime = inode->i_dtime = 0; // XXX
			inode->i_nlink = 1;
			inode->i_blocks = 1;
			inode->i_instance = instance;
			extra_data_procfs_t *extra = kmalloc(sizeof(extra_data_procfs_t));
			extra->pid = pid;
			extra->process = process;
			inode->i_fs_specific = extra;
			inode->i_fops = kmalloc(sizeof(open_file_operations_t));
			inode->i_fops->close = procfs_close; /*procfs_close;*/
			inode->i_fops->readdir = procfs_readdir;
			
			dentry_t *d = kmalloc(sizeof(dentry_t));
			char *n = kmalloc(strlen(name) + 1);
			strcpy(n, name);
			d->d_name = (const char*)n;
			d->d_inode = inode;
			return d;
		} else {
			return NULL;			 
		}	
	} else if (((extra_data_procfs_t*)(dentry->d_inode->i_fs_specific))->process != NULL) {
		unsigned int i;
		for (i = 0; i < sizeof(procfs_file_list)/sizeof(procfs_file_t); i++) {
			if (strcmp(procfs_file_list[i].filename, name) == 0) {
				inode_t *inode = kmalloc(sizeof(inode_t));
				inode->i_ino = 1;
				inode->i_mode = 0755;
				inode->i_uid = 0;
				inode->i_gid = 0;
				inode->i_size = 512;
				inode->i_atime = inode->i_ctime = inode->i_mtime = inode->i_dtime = 0; // XXX
				inode->i_nlink = 1;
				inode->i_blocks = 1;
				inode->i_instance = instance;
				extra_data_procfs_t *oldextra = dentry->d_inode->i_fs_specific;
				extra_data_procfs_t *extra = kmalloc(sizeof(extra_data_procfs_t));
				inode->i_fs_specific = extra;
				extra->pid = oldextra->pid;
				extra->process = oldextra->process;
				inode->i_fops = kmalloc(sizeof(open_file_operations_t));
				inode->i_fops->close = procfs_close; /*procfs_close;*/
				inode->i_fops->read = procfs_file_list[i].read;
		
				dentry_t *d = kmalloc(sizeof(dentry_t));
				char *n = kmalloc(strlen(name) + 1);
				strcpy(n, name);
				d->d_name = (const char*)n;
				d->d_inode = inode;
				return d;
			}
		}
	}

	return NULL;
}

/*
static int procfs_stat(inode_t *inode, struct stat *stbuf) {
	unsigned int i,j;
	int pid;
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
		pid = atoi(buf);
		if(find_process(pid) != NULL) {
			if(path[i] == '\0') {
				stbuf->st_mode |= S_IFDIR;
				return 0;
			}
			else {
				while (path[i] == '/') 
					i++;

				j=0;
				while(path[i] != '\0' && path[i] != '/') {
					buf[j] = path[i];
					i++;
					j++;
				}
				buf[j] = '\0';
				i=0;
				while(i<sizeof(procfs_file_list)/sizeof(procfs_file_t)) {
					if(strcmp(buf, procfs_file_list[i].filename) == 0) {
						stbuf->st_mode |= S_IFREG;
						return 0;
					}
					i++;
				}
				return -ENOENT;
			}
		}
	}
	// XXX: Absolete.


	return 0;
}
*/

static fs_instance_t* mount_procfs() {
	klog("mounting ProcFS");

	fs_instance_t *instance = kmalloc(sizeof(fs_instance_t));
	instance->mkdir = NULL;
//	instance->stat = procfs_stat;
	instance->getroot = procfs_getroot;
	instance->lookup = procfs_lookup;
	
	return instance;
}

static void umount_procfs(fs_instance_t *instance) {
	kfree(instance);
}

void procfs_init() {
	root_procfs.d_name = "";
	root_procfs.d_inode = kmalloc(sizeof(inode_t));
	root_procfs.d_inode->i_ino = 0;
	root_procfs.d_inode->i_mode = S_IFDIR | 00755;
	root_procfs.d_inode->i_fops = &procfs_fops;
	extra_data_procfs_t *extra = kmalloc(sizeof(extra_data_procfs_t));
	extra->pid = -1;
	extra->process = NULL;
	root_procfs.d_inode->i_fs_specific = extra;

	vfs_register_fs(&proc_fs);
}

