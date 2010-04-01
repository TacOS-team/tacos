#include <types.h>
#include <fcntl.h>
#include <process.h>
#include <stdio.h>
#include <string.h>

open_file_descriptor ofd;


void* sys_open(uint32_t p_process, uint32_t p_path, uint32_t flags) {
	int i;
	
	process_t * process = (process_t*) p_process;
	char * path = (char*) p_path;
	
	printf("sys_open\n");
	
	// on cherche une place dans la file descriptor table du process
	while(process->fd[i].used==TRUE) 
		i++;
	
	// on cree un open_file_descriptor
	process->fd[i].ofd = &ofd; // Au malloc quand il marchera......
	
	strcpy(process->fd[i].ofd->path, path);
	process->fd[i].ofd->flags = flags;
	
	/*
	strcpy(process->fd[i].ofdf>irst_cluster;
	strcpy(process->fd[i].ofd->current_cluster;
	strcpy(process->fd[i].ofd->buffer;
	strcpy(process->fd[i].ofd->current_octet;
	strcpy(process->fd[i].ofd->size_of_file;
	*/
  return NULL;
}

int open(const char *pathname, int flags) {
	
	
	process_t* process = get_current_process();
	
	
	
	syscall(3,process,pathname,flags);
}
