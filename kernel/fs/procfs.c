#include <vfs.h>
#include <kdirent.h>
#include <klog.h>
#include <kprocess.h>
#include <kmalloc.h>
#include <string.h>
#include <types.h>

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
	
	if (path[0] != '/')
		return NULL;
	else {
		i=0;
		while (path[i] == '/') 
			i++;
			
		if(path[i] == '\0') {
			/* Les fichier son sous un dossier pid, donc ici y'a rien à ouvrir */
			return NULL;
		}	
		else {
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
				/* On est dans un pid valide, alors on regarde le fichier à ouvrire, et on configure l'ofd en fonction */
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
						
						return ofd;
					}
					i++;
				}
				return NULL;
			} else {
				return NULL;
			}
		}
	}
}


int procfs_opendir(fs_instance_t *instance __attribute__((unused)), const char * path) {
	int i,j;
	int pid;
	char buf[64]; /* Beaucoup trop */
	
	if (path[0] == '\0')
		return 0;
	if (path[0] != '/')
		return -ENOENT;
		
	i=0;
	while (path[i] == '/') 
		i++;
	
	if(path[i] != '\0') { /* Dans ce cas on cherche un pid */
		j=0;
		while(path[i] != '\0' && path[i] != '/') {
			buf[j] = path[i];
			i++;
			j++;
		}
		buf[j] = '\0';
		pid = atoi(buf);
		if(find_process(pid) != NULL) {
			return 0;
		} else {
			return -ENOENT;
		}
	}
	
	return 0;
}

int procfs_readdir(fs_instance_t *instance __attribute__((unused)), const char * path, int iter, char * filename) {
	int i,j;
	int pid;
	char buf[64];
	klog("Path = %s %d", path, iter);
	
	if (path[0] != '/')
		return -ENOENT;
	else {
		i=0;
		while (path[i] == '/') 
			i++;
			
		if(path[i] == '\0') {
			i=0;
			while(i<MAX_PROC) {
				while(i<MAX_PROC && get_process_array(i) == NULL)
					i++;
				if(!iter && get_process_array(i) != NULL) {
					itoa (filename, 10, get_process_array(i)->pid);
					return 0;
				}
				iter--;
				i++;
			}
		}
		else {
			j=0;
			while(path[i] != '\0' && path[i] != '/') {
				buf[j] = path[i];
				i++;
				j++;
			}
			buf[j] = '\0';
			pid = atoi(buf);
			if(find_process(pid) != NULL) {
				if((unsigned int)iter<sizeof(procfs_file_list)/sizeof(procfs_file_t)) {
					strcpy(filename, procfs_file_list[iter].filename);
					return 0;
				}
			} else {
				return -ENOENT;
			}
		}
	}
	return 1;
}

fs_instance_t* mount_ProcFS() {
	klog("mounting ProcFS");

	fs_instance_t *instance = kmalloc(sizeof(fs_instance_t));
	instance->open = procfs_open_file;
	instance->mkdir = NULL;
	instance->readdir = procfs_readdir;
	instance->opendir = procfs_opendir;
	
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

