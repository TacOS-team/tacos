/**
 * @file procfs.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2015 TacOS developers.
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
 * Proc FS.
 */

#include <fs/procfs.h>
#include <kdirent.h>
#include <klibc/string.h>
#include <klog.h>
#include <kmalloc.h>
#include <kprocess.h>
#include <mmap.h>
#include <scheduler.h>
#include <kerrno.h>
#include <types.h>
#include <vfs.h>
#include <klibc/stdlib.h>
#include <klibc/ctype.h>
#include <memory.h>


/**
 * Unique id policy :
 *  each process has a range of PROCFS_PPROCESS_RANGE possible values for inode ids
 *  The begining of the range is computed by pid * PROCFS_PPROCESS_RANGE
 *  Then for each process, the vars ofset are :
 */

// process folder
#define procfs_root_process_offset 1
// cmd_line
#define procfs_cmd_line_offset     procfs_root_process_offset + 1
// state
#define procfs_state_offset        procfs_cmd_line_offset + 1
// cwd
#define procfs_cwd_offset          procfs_state_offset + 1
// ppid
#define procfs_ppid_offset         procfs_cwd_offset + 1
// maps
#define procfs_maps_offset 	   procfs_ppid_offset + 1
// fd
#define procfs_fd_offset           procfs_maps_offset + 1
// fd/* => taille de FOPEN_MAX
#define procfs_first_fd_offset     procfs_fd_offset + 1
// priority
#define procfs_stat_offset     procfs_first_fd_offset + FOPEN_MAX
#define procfs_priority_offset     procfs_priority_offset + 1

#define PROCFS_PROCESS_RANGE       60*1000;


static const int MAX_PID_LENGTH = 5;
static const int MAX_FD_LENGTH  = 6;


// File system informations
static fs_instance_t* mount_procfs();
static void umount_procfs(fs_instance_t *instance);

static dentry_t root_procfs;

static file_system_t proc_fs = {
	.name         = "ProcFS",
	.unique_inode = 0,
	.mount        = mount_procfs,
	.umount       = umount_procfs
};

static dentry_t * procfs_getroot() {
	return &root_procfs;
}								

// lookup fonctions
typedef dentry_t* (lookup_function_t)(struct _fs_instance_t *instance,
											struct _dentry_t* dentry,
											const char * name);

static lookup_function_t process_lookup;
static lookup_function_t process_fd_lookup;
static lookup_function_t root_lookup;


// readdir functions
typedef int (procfs_read_dir_function_t) (open_file_descriptor * ofd,
											char * entries,
											size_t size);
static procfs_read_dir_function_t procfs_read_root_dir;
static procfs_read_dir_function_t procfs_read_process_dir;
static procfs_read_dir_function_t procfs_read_fd_process_dir;

typedef union specific_extra_data_procfs_t {
	// For file descriptors list
	size_t fd;
} specific_extra_data_procfs_t;

/**
 *  Specific extra datas for procfs dentries
 */
typedef struct {
	int pid; /**< PID du process. */
	lookup_function_t * lookup;
	specific_extra_data_procfs_t specific_data;
} extra_data_procfs_t;


#ifndef EOF
# define EOF (-1)
#endif


static char* proc_status[]= {
					"IDLE",
					"RUNNING",
					"WAITING",
					"SUSPEND",
					"TERMINATED"
				};


/*****************************************************************************
 *                           procfs files functions                          *
 *****************************************************************************/

static ssize_t write_string_in_buffer(open_file_descriptor * ofd, char * dest,
                                      char * src, size_t count) {
	ssize_t result = EOF;
	size_t src_length = strlen(src);
	if (ofd->current_octet < src_length) {
		size_t remaining_bytes = src_length - ofd->current_octet;
		if (remaining_bytes > count) {
			result = count;
		} else {
			result = remaining_bytes;
		}
		memcpy(dest, src + ofd->current_octet, result);
		ofd->current_octet += result;
	}
	return result;
}

static char* memory_meminfo() {
	char *buffer = kmalloc(80);
	char *p = buffer;
	strcpy(p, "MemTotal:\t");
	p += 10;
	p += itoa(p, 10, memory_get_total() / 1024);
	strcpy(p, " kB\n");
	p += 4;
	strcpy(p, "MemFree:\t");
	p += 9;
	p += itoa(p, 10, memory_get_free() / 1024);
	strcpy(p, " kB\n");

	return buffer;
}

static ssize_t read_meminfo(open_file_descriptor *ofd, void* buffer, size_t count) {
	ssize_t result = EOF;
	char* meminfo = memory_meminfo();
	result = write_string_in_buffer(ofd, buffer, meminfo, count);
	kfree(meminfo);
	return result;
}

static ssize_t read_stat(open_file_descriptor *ofd, void* buffer, size_t count) {
	ssize_t result = EOF;
	char statinfo[80];

	sprintf(statinfo, "cpu %llu 0 %llu %llu\n", user_time, sys_time, idle_time);
	result = write_string_in_buffer(ofd, buffer, statinfo, count);
	return result;
}

static ssize_t procfs_read_name(open_file_descriptor * ofd, void* buffer, size_t count) {
	ssize_t result = EOF;
	extra_data_procfs_t * extra = ofd->i_fs_specific;
	process_t * process         = find_process(extra->pid);
	if(process) {
		result = write_string_in_buffer(ofd, buffer, process->name, count);
	}
	return result;
}

static uint32_t count_regions(process_t* process) {
	struct mmap_region* aux = process->list_regions;
	uint32_t count = 0;
	while (aux) {
		count++;
		aux = aux->next;
	}
	return count;
}

static char* memory_maps(process_t* process) {
	struct mmap_region* aux = process->list_regions;
	int region_count = count_regions(process);
	
	char* buffer = kmalloc(80 * region_count); /* XXX à la louche */
	char* p = buffer;
	
	while(aux != NULL) {
		/* Start address */
		itox(p, aux->addr);
		p += 8;

		/* : */
		strcpy(p, ":");
		p += 1;

		/* End address */
		itox(p, aux->addr + aux->length);
		p += 8;

		/* Space */
		strcpy(p, " ");
		p += 1;

		/* Offset */
		itox(p, aux->offset);
		p += 8;

		/* Space */
		strcpy(p, " ");
		p += 1;

		/* Path */
		if(aux->fd == -1) {
			strcpy(p, "[anonymous]");
			p += 11;
		} else if(process->fd[aux->fd] == NULL) {
			strcpy(p, "[fd lost]");
			p += 9;
		} else {
			strcpy(p, process->fd[aux->fd]->pathname);
			p += strlen(process->fd[aux->fd]->pathname);
		}

		/* EOL */
		strcpy(p,"\n");
		p += 1;

		aux = aux->next;
	}
	strcpy(p , "\0");
	return buffer;
}

static ssize_t procfs_read_maps(open_file_descriptor * ofd, void* buffer, size_t count) {
	ssize_t result = EOF;
	extra_data_procfs_t* extra = ofd->i_fs_specific;
	process_t* process = find_process(extra->pid);
	if(process) {
		char* maps = memory_maps(process);
		result = write_string_in_buffer(ofd, buffer, maps, count);
	}
	return result;
}

static ssize_t procfs_read_ppid(open_file_descriptor * ofd, void* buffer, size_t count) {
	ssize_t result = EOF;
	extra_data_procfs_t *extra = ofd->i_fs_specific;
	process_t* process = find_process(extra->pid);
	if(process) {
		char ppid[MAX_PID_LENGTH+1];
		itoa(ppid, 10, process->ppid);
		result = write_string_in_buffer(ofd, buffer, ppid, count);
	}
	return result;
}

static ssize_t procfs_read_stat(open_file_descriptor * ofd, void* buffer, size_t count) {
	ssize_t result = EOF;
	extra_data_procfs_t *extra = ofd->i_fs_specific;
	process_t* process = find_process(extra->pid);
	if(process) {
		char *state_char = "IRWST";
		char buf[100];
		sprintf(buf, "%d %s %c %llu %llu\n", process->pid, process->name, state_char[process->state], process->sys_time, process->user_time);
		result = write_string_in_buffer(ofd, buffer, buf, count);
	}
	return result;
}

/*
static ssize_t procfs_read_priority(open_file_descriptor * ofd, void* buffer, size_t count) {
	ssize_t result = EOF;
	extra_data_procfs_t *extra = ofd->i_fs_specific;
	process_t* process = find_process(extra->pid);
	if(process) {
		char priority[MAX_PID_LENGTH+1];
		itoa(priority, 10, process->priority);
		result = write_string_in_buffer(ofd, buffer, priority, count);
	}
	return result;
}*/

static ssize_t procfs_read_state(open_file_descriptor * ofd, void* buffer, size_t count) {
	ssize_t result = EOF;
	extra_data_procfs_t *extra = ofd->i_fs_specific;
	process_t* process = find_process(extra->pid);
	if(process) {
		result = write_string_in_buffer(ofd, buffer, proc_status[process->state], count);
	}
	return result;
}

static ssize_t procfs_read_cwd(open_file_descriptor * ofd, void* buffer, size_t count) {
	ssize_t result = EOF;
	extra_data_procfs_t *extra = ofd->i_fs_specific;
	process_t* process = find_process(extra->pid);
	if(process) {
		result = write_string_in_buffer(ofd, buffer, process->cwd, count);
	}
	return result;
}

static ssize_t procfs_read_process_fd(open_file_descriptor * ofd, void* buffer, size_t count) {
	ssize_t result = EOF;
	extra_data_procfs_t *extra = ofd->i_fs_specific;
	process_t* process = find_process(extra->pid);
	if(process && process->fd[extra->specific_data.fd]) {
		result = write_string_in_buffer(ofd, buffer,
		                                process->fd[extra->specific_data.fd]->pathname, count);
	}
	return result;
}

static ssize_t procfs_read_self(open_file_descriptor * ofd, void* buffer, size_t count) {
	ssize_t result = EOF;
	process_t* process = get_current_process();
	if(process) {
		char data[MAX_PID_LENGTH];
		itoa(data, 10, process->pid);
		result = write_string_in_buffer(ofd, buffer, data, count);
	}
	return result;
}

/*****************************************************************************
 *                        procfs directories functions                       *
 *****************************************************************************/

/**
 * List of files in a process directory
 */
typedef struct {
	char * name;
	size_t name_length;// init dans procfs_init
	ssize_t (*read)(open_file_descriptor *, void *, size_t);
	size_t inode_offset;
} procfs_file_function_t;

static procfs_file_function_t procfs_file_functions_list[] = {
	{ "cmd_line", 0, procfs_read_name,     procfs_cmd_line_offset },
	{ "ppid",     0, procfs_read_ppid,     procfs_ppid_offset },
	{ "state",    0, procfs_read_state,    procfs_state_offset },
	{ "cwd",      0, procfs_read_cwd,      procfs_cwd_offset },
	{ "maps",     0, procfs_read_maps,     procfs_maps_offset },
	{ "stat",     0, procfs_read_stat,     procfs_stat_offset }/*,
	{ "priority", 0, procfs_read_priority, procfs_priority_offset }*/
};

const size_t nb_file_functions = sizeof(procfs_file_functions_list) / sizeof(procfs_file_functions_list[0]);

/**
 *  List of directories in the process directory
 */
typedef struct {
	char * name;
	size_t name_length;// init dans procfs_init
	lookup_function_t * lookup;
	procfs_read_dir_function_t * readdir;
	size_t inode_offset;
} procfs_directory_function_t;

static procfs_directory_function_t procfs_directory_functions_list[] = {
	{ "fd", 0, process_fd_lookup, procfs_read_fd_process_dir, procfs_fd_offset }
};

static const size_t nb_directory_functions = sizeof(procfs_directory_functions_list)
															 		  / sizeof(procfs_directory_functions_list[0]);


/*****************************************************************************
 *                         read directories functions                        *
 *****************************************************************************/

static int procfs_read_root_dir(open_file_descriptor * ofd, char * entries, size_t size) {
	size_t count = 0;
	struct dirent *d;

	// On met une taille fixe à tous les dirent car on connait la taille max
	//  d'un PID
	size_t reclen = sizeof(d->d_ino)  + sizeof(d->d_reclen) + sizeof(d->d_type) + MAX_PID_LENGTH + 1;

	while (ofd->current_octet < MAX_PROC && count + reclen < size) {
		process_t * process = get_process_array(ofd->current_octet);
		if (process != NULL) {
			d = (struct dirent *)(entries + count);
			d->d_ino = 1;
			itoa(d->d_name, 10, process->pid);
			d->d_reclen = reclen;
			count += reclen;
		}
		++ofd->current_octet;
	}

	reclen = sizeof(d->d_ino)  + sizeof(d->d_reclen) + sizeof(d->d_type) + 8;
	if (ofd->current_octet == MAX_PROC && count + reclen < size) {
		d = (struct dirent *)(entries + count);
		d->d_ino = 2;
		strcpy(d->d_name, "self");
		d->d_reclen = reclen;
		count += reclen;
		++ofd->current_octet;
	}
	if (ofd->current_octet == MAX_PROC + 1 && count + reclen < size) {
		d = (struct dirent *)(entries + count);
		d->d_ino = 4;
		strcpy(d->d_name, "meminfo");
		// XXX: ce reclen me parait étrange...
		d->d_reclen = reclen;
		count += reclen;
		++ofd->current_octet;
	}
	if (ofd->current_octet == MAX_PROC + 2 && count + reclen < size) {
		d = (struct dirent *)(entries + count);
		d->d_ino = 5;
		strcpy(d->d_name, "stat");
		d->d_reclen = reclen;
		count += reclen;
		++ofd->current_octet;
	}


	return count;
}

static int procfs_read_process_dir(open_file_descriptor * ofd, char * entries,
																	 size_t size) {
	size_t count   = 0;
	size_t i;

	struct dirent *d;

	const size_t base_reclen = sizeof(d->d_ino)  + sizeof(d->d_reclen)
													 + sizeof(d->d_type) + 1;

	extra_data_procfs_t * extra = ofd->i_fs_specific;

	if (find_process(extra->pid) != NULL) {
		i = 0;
		while (ofd->current_octet < nb_file_functions
						&& count + procfs_file_functions_list[i].name_length < size) {
			i = ofd->current_octet;
			d = (struct dirent *)(entries + count);
			d->d_ino = 1;
			strcpy(d->d_name, procfs_file_functions_list[i].name);
			d->d_reclen = base_reclen + procfs_file_functions_list[i].name_length;
			count      += d->d_reclen;
			++ofd->current_octet;
			++i;
		}
		i = 0;
		while (ofd->current_octet - nb_file_functions < nb_directory_functions
						&& count + procfs_directory_functions_list[i].name_length < size) {
			d = (struct dirent *)(entries + count);
			d->d_ino = 1;
			strcpy(d->d_name, procfs_directory_functions_list[i].name);
			d->d_reclen = base_reclen + procfs_directory_functions_list[i].name_length;
			count      += d->d_reclen;
			++ofd->current_octet;
			++i;
		}
	}

	return count;
}

static int procfs_read_fd_process_dir(open_file_descriptor * ofd, char * entries,
																	 		size_t size) {
	size_t count   = 0;
	struct dirent *d;

	// On met une taille fixe à tous les dirent car on connait la taille max
	//  d'un FD
	const size_t reclen = sizeof(d->d_ino)  + sizeof(d->d_reclen)
											+ sizeof(d->d_type) + MAX_FD_LENGTH + 1;

	extra_data_procfs_t * extra = ofd->i_fs_specific;

	process_t * process = find_process(extra->pid);
	if (process != NULL) {
		while (ofd->current_octet < FOPEN_MAX && count + reclen < size) {
			if (process->fd[ofd->current_octet] != NULL) {
				d = (struct dirent *)(entries + count);
				d->d_ino = 1;
				itoa (d->d_name, 10, ofd->current_octet);
				d->d_reclen = reclen;
				count += reclen;
			}
			++ofd->current_octet;
		}
	}
	return count;
}



/*****************************************************************************
 *                             dentries créations                            *
 *****************************************************************************/

static dentry_t * get_default_dentry(struct _fs_instance_t * instance,
															const char * name, int pid) {
	inode_t * inode   = kmalloc(sizeof(inode_t));
	memset(inode, 0, sizeof(*inode));
	inode->i_ino      = pid * PROCFS_PROCESS_RANGE;
	// inode->i_uid      = 0;
	// inode->i_gid      = 0;
	inode->i_size     = 512;
	// inode->i_atime    = 0;
	// inode->i_ctime    = 0;
	// inode->i_mtime    = 0;
	// inode->i_dtime    = 0; // XXX
	inode->i_nlink    = 1;
	inode->i_blocks   = 1;
	inode->i_instance = instance;
	inode->i_count = 0;

	extra_data_procfs_t * extra = kmalloc(sizeof(extra_data_procfs_t));
	extra->pid    = pid;
	extra->lookup = NULL;
	inode->i_fs_specific = extra;

	inode->i_fops = kmalloc(sizeof(open_file_operations_t));
	memset(inode->i_fops, 0, sizeof(*(inode->i_fops)));
	inode->i_fops->seek = generic_seek;
	
	dentry_t * d = kmalloc(sizeof(dentry_t));
	d->d_name = (const char*) strdup(name);
	d->d_inode = inode;
	d->d_pdentry = NULL; //FIXME: Normalement il faudrait le faire à chaque niveau et appeler le père à chaque fois.

	return d;
}

static dentry_t * get_process_dentry(struct _fs_instance_t * instance,
															const char * name, int pid) {
	dentry_t * result = get_default_dentry(instance, name, pid);
	inode_t * inode   = result->d_inode;

	inode->i_ino     += procfs_root_process_offset;
	inode->i_mode     = 0555 | S_IFDIR;

	extra_data_procfs_t *extra = inode->i_fs_specific;
	extra->lookup = process_lookup;

	inode->i_fops->readdir = procfs_read_process_dir;

	return result;
}

static dentry_t * get_self_dentry(struct _fs_instance_t * instance) {

	inode_t * inode   = kmalloc(sizeof(inode_t));
	memset(inode, 0, sizeof(*inode));
	inode->i_ino      = MAX_PROC * PROCFS_PROCESS_RANGE;
	inode->i_size     = MAX_PID_LENGTH; // XXX
	// inode->i_atime    = 0;
	// inode->i_ctime    = 0;
	// inode->i_mtime    = 0;
	// inode->i_dtime    = 0; // XXX
	inode->i_nlink    = 1;
	inode->i_blocks   = 1;
	inode->i_instance = instance;
	inode->i_count = 0;

	inode->i_fops = kmalloc(sizeof(open_file_operations_t));
	memset(inode->i_fops, 0, sizeof(*(inode->i_fops)));
	inode->i_fops->read = procfs_read_self;
	
	dentry_t * d = kmalloc(sizeof(dentry_t));
	d->d_name = (const char*) strdup("self");
	d->d_inode = inode;
	d->d_pdentry = NULL; //FIXME: Normalement il faudrait le faire à chaque niveau et appeler le père à chaque fois.

	inode->i_ino     += procfs_root_process_offset;
	inode->i_mode     = 0444 | S_IFLNK;

	return d;
}

static dentry_t * get_process_fd_dentry(struct _fs_instance_t * instance,
															const char * name, int pid, size_t fd) {
	dentry_t * result = get_default_dentry(instance, name, pid);
	inode_t * inode   = result->d_inode;

	// TODO y réfléchir pour qu'ils soient vraiment uniques
	inode->i_ino     += procfs_first_fd_offset + fd;
	inode->i_mode     = 0555 | S_IFLNK ;

	extra_data_procfs_t *extra = inode->i_fs_specific;
	extra->lookup = NULL;
	extra->specific_data.fd = fd;

	inode->i_fops->read = procfs_read_process_fd;

	return result;
}

static dentry_t * get_file_function_dentry(struct _fs_instance_t * instance, const char * name, int pid, size_t inode_offset) {
	dentry_t * result = get_default_dentry(instance, name, pid);
	inode_t * inode   = result->d_inode;

	inode->i_ino = inode_offset;

	extra_data_procfs_t *extra = inode->i_fs_specific;
	extra->lookup = NULL;

    if (strcmp(name, "cwd") == 0) {
	    inode->i_mode = 0555 | S_IFLNK;
    } else {
	    inode->i_mode = 0555 | S_IFREG;
    }

	return result;
}

static dentry_t * get_directory_function_dentry(struct _fs_instance_t * instance,
                                                const char * name, int pid, lookup_function_t lookup_function,
                                                size_t inode_offset) {
	dentry_t * result = get_default_dentry(instance, name, pid);
	inode_t * inode   = result->d_inode;

	inode->i_ino     += inode_offset;

	extra_data_procfs_t *extra = inode->i_fs_specific;
	extra->lookup = lookup_function;

	inode->i_mode     = 0555 | S_IFDIR;

	return result;
}

static dentry_t * get_meminfo_dentry(struct _fs_instance_t *instance) {
	inode_t * inode   = kmalloc(sizeof(inode_t));
	memset(inode, 0, sizeof(*inode));
	inode->i_ino      = 3; // XXX: devrait etre unique et non juste arbitraire :)
	inode->i_size     = 512; // XXX
	// inode->i_atime    = 0;
	// inode->i_ctime    = 0;
	// inode->i_mtime    = 0;
	// inode->i_dtime    = 0; // XXX
	inode->i_nlink    = 1;
	inode->i_blocks   = 1;
	inode->i_instance = instance;
	inode->i_count = 0;
	inode->i_mode     = 0444 | S_IFREG;

	inode->i_fs_specific = NULL;

	inode->i_fops = kmalloc(sizeof(open_file_operations_t));
	memset(inode->i_fops, 0, sizeof(*(inode->i_fops)));
	inode->i_fops->read = read_meminfo;
	inode->i_fops->seek = generic_seek;
	
	dentry_t * d = kmalloc(sizeof(dentry_t));
	d->d_name = (const char*) strdup("meminfo");
	d->d_inode = inode;
	d->d_pdentry = NULL; //FIXME

	return d;
}

static dentry_t * get_stat_dentry(struct _fs_instance_t *instance) {
	inode_t * inode   = kmalloc(sizeof(inode_t));
	memset(inode, 0, sizeof(*inode));
	inode->i_ino      = 5; // XXX: devrait etre unique et non juste arbitraire :)
	inode->i_size     = 512; // XXX
	// inode->i_atime    = 0;
	// inode->i_ctime    = 0;
	// inode->i_mtime    = 0;
	// inode->i_dtime    = 0; // XXX
	inode->i_nlink    = 1;
	inode->i_blocks   = 1;
	inode->i_instance = instance;
	inode->i_count = 0;
	inode->i_mode     = 0444 | S_IFREG;

	inode->i_fs_specific = NULL;

	inode->i_fops = kmalloc(sizeof(open_file_operations_t));
	memset(inode->i_fops, 0, sizeof(*(inode->i_fops)));
	inode->i_fops->read = read_stat;
	
	dentry_t * d = kmalloc(sizeof(dentry_t));
	d->d_name = (const char*) strdup("stat");
	d->d_inode = inode;
	d->d_pdentry = NULL; //FIXME

	return d;
}


/*****************************************************************************
 *                              lookup functions                             *
 *****************************************************************************/

static dentry_t * root_lookup(struct _fs_instance_t *instance,
                              struct _dentry_t* dentry __attribute__((unused)),
                              const char * name) {
	dentry_t * result = NULL;
	const char *str = name;
	while (*str)
	{
		if (!isdigit(*str))
			break;
		else
			++str;
	}

	if (*str) {
		if (strcmp(name, "self") == 0) {
			result = get_self_dentry(instance);
		} else if (strcmp(name, "meminfo") == 0) {
			result = get_meminfo_dentry(instance);
		} else if (strcmp(name, "stat") == 0) {
			result = get_stat_dentry(instance);
		}
	} else {
		int pid = atoi(name);
		process_t * process = find_process(pid);
		if (process != NULL) {
			result = get_process_dentry(instance, name, pid);
		}
	}
	return result;
}

static dentry_t * process_lookup(struct _fs_instance_t *instance, struct _dentry_t* dentry, const char * name) {
	extra_data_procfs_t * extra = dentry->d_inode->i_fs_specific;
	dentry_t * result = NULL;
	size_t i;
	for (i = 0; i < nb_file_functions && result == NULL; ++i) {
		if (strcmp(procfs_file_functions_list[i].name, name) == 0) {
			result = get_file_function_dentry(instance, name, extra->pid, procfs_file_functions_list[i].inode_offset);
			result->d_inode->i_fops->read = procfs_file_functions_list[i].read;
		}
	}
	for (i = 0; i < nb_directory_functions && result == NULL; ++i) {
		if (strcmp(procfs_directory_functions_list[i].name, name) == 0) {
			result = get_directory_function_dentry(instance, name, extra->pid,
																		procfs_directory_functions_list[i].lookup,
																		procfs_directory_functions_list[i].inode_offset);
			result->d_inode->i_fops->readdir = procfs_directory_functions_list[i].readdir;
		}
	}
	return result;
}

static dentry_t * process_fd_lookup(struct _fs_instance_t *instance,
																 struct _dentry_t* dentry,
																 const char * name) {
	dentry_t * result = NULL;
	extra_data_procfs_t * extra = dentry->d_inode->i_fs_specific;
	process_t * process = find_process(extra->pid);
	size_t num_fd = atoi(name);
	if (process && num_fd < FOPEN_MAX && process->fd[num_fd]) {
		result = get_process_fd_dentry(instance, name, extra->pid, num_fd);
	}
	return result;
}


static dentry_t * procfs_lookup(struct _fs_instance_t *instance,
																struct _dentry_t* dentry, const char * name) {
	dentry_t * result = NULL;
	extra_data_procfs_t * extra = dentry->d_inode->i_fs_specific;
	if (extra != NULL && extra->lookup != NULL) {
		result = extra->lookup(instance, dentry, name);
	}
	return result;
}

static fs_instance_t* mount_procfs() {
	klog("mounting ProcFS");

	fs_instance_t *instance = kmalloc(sizeof(fs_instance_t));
	memset(instance, 0, sizeof(fs_instance_t));
	instance->getroot = procfs_getroot;
	instance->lookup = procfs_lookup;
	instance->fs = &proc_fs;
	
	return instance;
}

static void umount_procfs(fs_instance_t *instance) {
	kfree(instance);
}

void procfs_init() {
	size_t i;
	// Initialise la taille des noms de fonctions
	for (i = 0; i < nb_file_functions; ++i) {
		procfs_file_functions_list[i].name_length
				= strlen(procfs_file_functions_list[i].name);
	}
	for (i = 0; i < nb_directory_functions; ++i) {
		procfs_directory_functions_list[i].name_length
				= strlen(procfs_directory_functions_list[i].name);
	}

	root_procfs.d_name = "proc";
	root_procfs.d_pdentry = NULL;

	root_procfs.d_inode = kmalloc(sizeof(inode_t));
	memset(root_procfs.d_inode, 0, sizeof(*(root_procfs.d_inode)));
	root_procfs.d_inode->i_ino  = 0;
	root_procfs.d_inode->i_mode = S_IFDIR | 00555;

	root_procfs.d_inode->i_fops = kmalloc(sizeof(struct _open_file_operations_t));
	memset(root_procfs.d_inode->i_fops, 0, sizeof(*(root_procfs.d_inode->i_fops)));
	root_procfs.d_inode->i_fops->readdir = procfs_read_root_dir;
	root_procfs.d_inode->i_fops->seek = generic_seek;

	extra_data_procfs_t *extra = kmalloc(sizeof(extra_data_procfs_t));
	extra->pid    = -1;
	extra->lookup = root_lookup;

	root_procfs.d_inode->i_fs_specific = extra;

	vfs_register_fs(&proc_fs);
}
