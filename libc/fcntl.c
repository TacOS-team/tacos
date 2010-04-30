#include <types.h>
#include <fcntl.h>
#include <process.h>
#include <stdio.h>
#include <string.h>
#include <syscall.h>
#include <fat.h>
open_file_descriptor ofd;

/* TODO: enlever les  attribute quand les paramètre serviront à quelque chose */
void* sys_open(uint32_t p_process __attribute__ ((unused)), uint32_t p_path , uint32_t flags __attribute__ ((unused))) {
	int i=0;
	
	//process_t * process = (process_t*) p_process;
	char * path = (char*) p_path;
	
	printf("sys_open\n");
	
	process_t* process = get_current_process();
	printf("current process ring0: %d\n",(uint32_t)process);
	
	// recherche d une place dans la file descriptor table du process
	while(process->fd[i].used==TRUE) 
		i++;
	
	// creation d un open_file_descriptor
	process->fd[i].ofd = &ofd; //malloc(sizeof(open_file_descriptor));
	
	//ouverture du fichier (sur fd0 pour le momentt)
	fat_open_file(path, process->fd[i].ofd);
	
  return NULL;
}

int open(const char *pathname, int flags) {
	
	syscall(3,0,(paddr_t) pathname,flags);
	
	return 0;
}
