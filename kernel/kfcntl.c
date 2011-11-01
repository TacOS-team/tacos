/**
 * @file kfcntl.c
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
 * Description de ce que fait le fichier
 */

#include <kprocess.h>
#include <ksyscall.h>
#include <scheduler.h>
#include <stdio.h>
#include <kfat.h>
#include <string.h>
#include <kmalloc.h>

#include <kdriver.h>
#include <keyboard.h>
#include <console.h>
#include <tty.h>
#include <klog.h>

#include <fcntl.h>

void init_stdfd(process_t *new_proc) {
    struct _file_descriptor *fd0 = &(new_proc->fd[0]);
    struct _file_descriptor *fd1 = &(new_proc->fd[1]);
    struct _file_descriptor *fd2 = &(new_proc->fd[2]);
    terminal_t * t;

    if (new_proc->ppid == 0) { // pas de père, on peut pas récupérer son tty.
				int t_i = tty_alloc();
				t = tty_get(t_i);
        int console = get_available_console(t);
        focus_console(console);
        tty_init(t, new_proc, (void *)console,(void (*)(void *, char)) kputchar);
        new_proc->ctrl_tty = t_i;
    } else {
        process_t *pprocess = get_process(new_proc->ppid);
        int t_i = pprocess->ctrl_tty;
				t = tty_get(t_i);
				new_proc->ctrl_tty = t_i;
				tty_set_fg_process(t, new_proc);
        //focus_console
    }
	fd0->used = TRUE; /* stdin */
	fd0->ofd = kmalloc(sizeof(open_file_descriptor));
  fd0->ofd->flags = O_RDONLY;
	fd0->ofd->read = tty_read;
	fd0->ofd->extra_data = t;
	fd0->ofd->ioctl = tty_ioctl;
	fd1->used = TRUE; /* stdout */
	fd1->ofd = kmalloc(sizeof(open_file_descriptor));
  fd1->ofd->flags = O_WRONLY;
	fd1->ofd->write = tty_write;
	fd1->ofd->ioctl = tty_ioctl;
	fd1->ofd->extra_data = t;
	fd2->used = TRUE; /* stderr */
	fd2->ofd = kmalloc(sizeof(open_file_descriptor));
  fd2->ofd->flags = O_WRONLY;
	fd2->ofd->write = tty_write;
	fd2->ofd->ioctl = tty_ioctl;
	fd2->ofd->extra_data = t;
}

void close_all_fd() {
	int fd_id;
	process_t * process = get_current_process();
	open_file_descriptor *ofd;

	for (fd_id = 0; fd_id < FOPEN_MAX; fd_id++) {
		if (process->fd[fd_id].used) {
			ofd = process->fd[fd_id].ofd;

			if(ofd->close != NULL)
				ofd->close(ofd);
		}
	}
}


SYSCALL_HANDLER3(sys_open, uint32_t fd_id, uint32_t p_path , uint32_t flags) {
	int i=0;
	driver_interfaces* di = NULL;
	
	//process_t * process = (process_t*) p_process;
	char* path = (char*) p_path;
	process_t* process = get_current_process();
	
	// recherche d une place dans la file descriptor table du process
	while(process->fd[i].used==TRUE) 
		i++;
	
	// creation d un open_file_descriptor
	process->fd[i].ofd = kmalloc(sizeof(open_file_descriptor));
	process->fd[i].used = TRUE;
	process->fd[i].ofd->flags = flags;
	
	// ouverture du fichier (sur fd0 pour le moment)
	if(path[0] == '$')
	{
		di = find_driver(path+1);
		if(di != NULL)
		{
			if( di->open == NULL )
				kerr("No \"open\" method for device \"%s\".", path+1);
			else
				di->open(process->fd[i].ofd);
				
			process->fd[i].ofd->write = di->write;
			process->fd[i].ofd->read = di->read;
			process->fd[i].ofd->seek = di->seek;
			process->fd[i].ofd->ioctl = di->ioctl;
			process->fd[i].ofd->open = di->open;
			process->fd[i].ofd->close = di->close;
			process->fd[i].ofd->flush = di->flush;
			
			*((int *)fd_id) = i;
		}
		else
			*((int *)fd_id) = -1;
	}
	else if (fat_open_file(path, process->fd[i].ofd, flags) == 0) {
		// C'est ici qu'on devrait vérifier si le fichier ouvert est spécial 
		// pour savoir si on doit binder au file system ou à un driver.
		process->fd[i].ofd->write = write_file;
		process->fd[i].ofd->read = read_file;
		process->fd[i].ofd->seek = seek_file;
		*((int *)fd_id) = i;
	} else {
		*((int *)fd_id) = -1;
	}
}

SYSCALL_HANDLER2(sys_close, uint32_t fd_id, uint32_t* ret)
{
	process_t * process = get_current_process();
	*ret = -1;
	open_file_descriptor *ofd;

	if (process->fd[fd_id].used) {
		ofd = process->fd[fd_id].ofd;
	
		if(ofd->close == NULL)
			kerr("No \"close\" method for this device.");
		else
			*ret = ofd->close(ofd);
	}
}
