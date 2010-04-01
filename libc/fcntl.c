#include <types.h>
#include <fcntl.h>
#include <process.h>
#include <stdio.h>

open_file_descriptor ofd;


void* sys_open(uint32_t p_ofd, uint32_t zero1, uint32_t zero2)
{
	printf("sys_open\n");

  return NULL;
}

int open(const char *pathname, int flags) {
	int i;
	
	process_t* process = get_current_process();
	
	// on cherche une place dans la file descriptor table du process
	while(process->fd[i].used==TRUE) 
		i++;
	
	// on cree un open_file_descriptor
	process->fd[i].ofd = &ofd; // Au malloc quand il marchera......
	
	syscall(3,&ofd,0,0);
}
