/**
 * @file fcntl.c
 */

#include <types.h>
#include <fcntl.h>
#include <process.h>
#include <stdio.h>
#include <string.h>
#include <syscall.h>
#include <fat.h>
#include <video.h>
#include <stdlib.h>
#include <keyboard.h>

void init_stdfd(struct _file_descriptor *fd0, struct _file_descriptor *fd1, struct _file_descriptor *fd2) {
	fd0->used = TRUE; /* stdin */
	fd0->ofd = malloc(sizeof(open_file_descriptor));
	fd0->ofd->write = write_keyboard;
	fd0->ofd->read = read_screen;
	fd0->ofd->current_octet_buf = 0;
	fd1->used = TRUE; /* stdout */
	fd1->ofd = malloc(sizeof(open_file_descriptor));
	fd1->ofd->write = write_screen;
	fd1->ofd->extra_data = creation_text_window(0, 0, 80, 24, 0, 0, 0, DEFAULT_ATTRIBUTE_VALUE, 0);
	fd2->used = TRUE; /* stderr */
	fd2->ofd = malloc(sizeof(open_file_descriptor));
	fd2->ofd->write = write_screen;
	fd2->ofd->extra_data = creation_text_window(0, 0, 80, 24, 0, 0, 0, DEFAULT_ATTRIBUTE_VALUE, 1);
}


/* TODO: enlever les  attribute quand les paramètre serviront à quelque chose */
void* sys_open(uint32_t fd_id, uint32_t p_path , uint32_t flags __attribute__ ((unused))) {
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
	process->fd[i].ofd = malloc(sizeof(open_file_descriptor));
	
	//ouverture du fichier (sur fd0 pour le momentt)
	fat_open_file(path, process->fd[i].ofd);
	

	*((int *)fd_id) = i;

  return NULL;
}

int open(const char *pathname, int flags) {
	int id;
	syscall(SYS_OPEN,(uint32_t)&id,(paddr_t) pathname,flags);
	
	return id;
}
