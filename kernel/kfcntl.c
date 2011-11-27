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

#include <klog.h>
#include <kmalloc.h>
#include <kprocess.h>
#include <ksyscall.h>
#include <scheduler.h>
#include <string.h>
#include <tty.h>
#include <vfs.h>

#include <fcntl.h>

void init_stdfd(process_t *new_proc) {
    struct _file_descriptor *fd0 = &(new_proc->fd[0]);
    struct _file_descriptor *fd1 = &(new_proc->fd[1]);
    struct _file_descriptor *fd2 = &(new_proc->fd[2]);
    tty_struct_t * t;

    if (new_proc->ppid == 0) { // pas de père, on peut pas récupérer son tty.
				open_file_descriptor *t_i = vfs_open("/dev/tty0", 0); //XXX: flags.
				t =  t_i->extra_data;
        new_proc->ctrl_tty = t;
    } else {
        process_t *pprocess = find_process(new_proc->ppid);
        t = pprocess->ctrl_tty;
				new_proc->ctrl_tty = t;
				//tty_set_fg_process(t, new_proc);
    }
		t->fg_process = new_proc->pid;
		//XXX:Beurk.
	fd0->used = true; /* stdin */
	fd0->ofd = kmalloc(sizeof(open_file_descriptor));
  fd0->ofd->flags = O_RDONLY;
	fd0->ofd->read = tty_read;
	fd0->ofd->extra_data = t;
	fd0->ofd->ioctl = tty_ioctl;
	fd0->ofd->close = tty_close;
	fd1->used = true; /* stdout */
	fd1->ofd = kmalloc(sizeof(open_file_descriptor));
  fd1->ofd->flags = O_WRONLY;
	fd1->ofd->write = tty_write;
	fd1->ofd->ioctl = tty_ioctl;
	fd1->ofd->close = tty_close;
	fd1->ofd->extra_data = t;
	fd2->used = true; /* stderr */
	fd2->ofd = kmalloc(sizeof(open_file_descriptor));
  fd2->ofd->flags = O_WRONLY;
	fd2->ofd->write = tty_write;
	fd2->ofd->ioctl = tty_ioctl;
	fd2->ofd->close = tty_close;
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
	
	//process_t * process = (process_t*) p_process;
	char* path = (char*) p_path;
	process_t* process = get_current_process();
	
	// recherche d une place dans la file descriptor table du process
	while(process->fd[i].used==true) 
		i++;

	// ouverture du fichier
	if ((process->fd[i].ofd = vfs_open(path, flags)) != NULL) {
		*((int *)fd_id) = i;
		process->fd[i].used = true;
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
		kfree(ofd);
		process->fd[fd_id].used = 0;
	}
}

SYSCALL_HANDLER3(sys_fcntl, int *fd_id, unsigned int request, void * data) {
	process_t * process = get_current_process();
	open_file_descriptor *ofd;
	if (process->fd[*fd_id].used) {
		ofd = process->fd[*fd_id].ofd;

		if (request == F_SETFL) {
			ofd->flags = (int)data;
			*fd_id = 0;
			return;
		}
	}
	*fd_id = -1;
}


