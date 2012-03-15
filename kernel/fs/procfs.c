#include <vfs.h>
#include <kdirent.h>
#include <klog.h>
#include <kprocess.h>
#include <kmalloc.h>
#include <string.h>
#include <types.h>
#include <fs/procfs.h>

#include <stdlib.h>

typedef struct {
	char* filename;
	size_t (*read)(open_file_descriptor *,void*, size_t);
}procfs_file_t;

#ifndef EOF
# define EOF (-1)
#endif

char* proc_status[]= {"IDLE", "RUNNING", "WAITING", "SUSPEND", "TERMINATED"};

static size_t procfs_read_name(open_file_descriptor* ofd, void* buffer, size_t count __attribute__((unused))) {
	process_t* process = ofd->extra_data;
	if(ofd->current_octet == 0) {
		strcpy(buffer,process->name);
		ofd->current_octet = strlen(process->name);
		return ofd->current_octet;
	}
	else
		return EOF;
}

static size_t procfs_read_ppid(open_file_descriptor* ofd, void* buffer, size_t count __attribute__((unused))) {
	process_t* process = ofd->extra_data;
	if(ofd->current_octet == 0) {
		itoa(buffer, 10, process->ppid);
		ofd->current_octet = strlen(buffer);
		return ofd->current_octet;
	}
	else
		return EOF;
}

static size_t procfs_read_state(open_file_descriptor* ofd, void* buffer, size_t count __attribute__((unused))) {
	process_t* process = ofd->extra_data;
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

open_file_descriptor* procfs_open_file(fs_instance_t *instance, const char * path, uint32_t flags __attribute__((unused))) {
	unsigned int i,j;
	int pid;
	char buf[64];
	process_t* process = NULL;
	
	i = 0;
	while (path[i] == '/') 
		i++;
		
	if(path[i] == '\0') {
		open_file_descriptor *ofd = kmalloc(sizeof(open_file_descriptor));
		
		ofd->flags = flags;
		ofd->fs_instance = instance;
		ofd->first_cluster = 0;
		ofd->file_size = 512; /* TODO */
		ofd->current_cluster = 0;
		ofd->current_octet = 0;
		ofd->current_octet_buf = 0;

		ofd->readdir = procfs_readdir;

		return ofd;
	}	else {
		j=0;
		while(path[i] != '\0' && path[i] != '/') {
			buf[j] = path[i];
			i++;
			j++;
		}
		buf[j] = '\0';
		pid = atoi(buf);
		process = find_process(pid);
		if(process != NULL) {
			/* On est dans un pid valide, alors on regarde le fichier à ouvrir, et on configure l'ofd en fonction */
			while (path[i] == '/') 
				i++;

			j=0;
			while(path[i] != '\0' && path[i] != '/') {
				buf[j] = path[i];
				i++;
				j++;
			}
			buf[j] = '\0';

			if (j > 0) {
				i=0;
				while(i<sizeof(procfs_file_list)/sizeof(procfs_file_t)) {
					if(strcmp(buf, procfs_file_list[i].filename) == 0) {
						open_file_descriptor *ofd = kmalloc(sizeof(open_file_descriptor));

						ofd->fs_instance = instance;
						ofd->first_cluster = 0;
						ofd->file_size = 512; /* TODO */
						ofd->current_cluster = ofd->first_cluster;
						ofd->current_octet = 0;
						ofd->current_octet_buf = 0;
						
						ofd->extra_data = process;
						ofd->write = NULL;/* procfs_write_file;*/
						ofd->read = procfs_file_list[i].read; /*procfs_read_file; */
						ofd->seek = NULL; /*procfs_seek_file; */
						ofd->close = procfs_close; /*procfs_close;*/
						ofd->readdir = NULL;
						
						return ofd;
					}
					i++;
				}
			} else {
				open_file_descriptor *ofd = kmalloc(sizeof(open_file_descriptor));

				ofd->fs_instance = instance;
				ofd->first_cluster = pid;
				ofd->file_size = 512; /* TODO */
				ofd->current_cluster = ofd->first_cluster;
				ofd->current_octet = 0;
				ofd->current_octet_buf = 0;
				
				ofd->extra_data = process;
				ofd->write = NULL;/* procfs_write_file;*/
				ofd->seek = NULL; /*procfs_seek_file; */
				ofd->read = NULL;
				ofd->close = procfs_close; /*procfs_close;*/
				ofd->readdir = procfs_readdir;
				
				return ofd;
			}
			return NULL;
		} else {
			return NULL;
		}
	}
}

int procfs_readdir(open_file_descriptor * ofd, char * entries, size_t size) {
	size_t count = 0;
	unsigned int i = ofd->current_octet;

	if (ofd->current_cluster == 0) {
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
	} else {
		int pid = ofd->current_cluster;
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

int procfs_stat(fs_instance_t *instance __attribute__ ((unused)), const char *path, struct stat *stbuf) {
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
	
	return 0;
}

fs_instance_t* mount_ProcFS() {
	klog("mounting ProcFS");

	fs_instance_t *instance = kmalloc(sizeof(fs_instance_t));
	instance->open = procfs_open_file;
	instance->mkdir = NULL;
	instance->stat = procfs_stat;
	
	return instance;
}

void umount_ProcFS(fs_instance_t *instance) {
	kfree(instance);
}

void procfs_init() {
	file_system_t *fs = kmalloc(sizeof(file_system_t));
	fs->name = "ProcFS";
	fs->mount = mount_ProcFS;
	fs->umount = umount_ProcFS;
	vfs_register_fs(fs);
}

