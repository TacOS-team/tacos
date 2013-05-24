/**
 * @file kfcntl.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012, 2013 - TacOS developers.
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

#include <fs/devfs.h>
#include <kfcntl.h>
#include <klibc/string.h>
#include <klog.h>
#include <kmalloc.h>
#include <kprocess.h>
#include <ksyscall.h>
#include <scheduler.h>
#include <tty.h>
#include <vfs.h>

void init_stdfd(process_t *new_proc) {
	if (new_proc->pid == 0)
		return;

	process_t *pprocess = NULL;

	if (new_proc->ppid > 0) {
		pprocess = find_process(new_proc->ppid);
	}

	if (pprocess) { //XXX: Je n'assume pas la paternité de ces quelques lignes (ahah).
		new_proc->ctrl_tty = pprocess->ctrl_tty;
		int i;
		for (i = 0; i < FOPEN_MAX; i++) {
			if (pprocess->fd[i]) {
				new_proc->fd[i] = kmalloc(sizeof(struct _open_file_descriptor));
				memcpy(new_proc->fd[i], pprocess->fd[i], sizeof(struct _open_file_descriptor));
				new_proc->fd[i]->pathname = kmalloc(strlen(pprocess->fd[i]->pathname) + 1);
				strcpy(new_proc->fd[i]->pathname, pprocess->fd[i]->pathname);
				new_proc->fd[i]->dentry->d_inode->i_count++;
			} else {
				new_proc->fd[i] = NULL;
			}
		}
	} else {
		new_proc->ctrl_tty = "/dev/tty0";

		/* stdin */
		new_proc->fd[0] = vfs_open(new_proc->ctrl_tty, O_RDONLY);
		/* stdout */
		new_proc->fd[1] = vfs_open(new_proc->ctrl_tty, O_WRONLY);
		/* stderr */
		new_proc->fd[2] = vfs_open(new_proc->ctrl_tty, O_WRONLY);
	}

	chardev_interfaces *di;
	open_file_descriptor *t_i = vfs_open(new_proc->ctrl_tty, 0); //XXX: flags.
	di = t_i->extra_data;
	tty_struct_t *t = di->custom_data;
	t->fg_process = new_proc->pid;
}

void close_all_fd() {
	int fd_id;
	process_t * process = get_current_process();
	open_file_descriptor *ofd;

	for (fd_id = 0; fd_id < FOPEN_MAX; fd_id++) {
		if (process->fd[fd_id]) {
			ofd = process->fd[fd_id];

			if(ofd->f_ops->close != NULL)
				ofd->f_ops->close(ofd);

			vfs_close(ofd);
			process->fd[fd_id] = NULL;
		}
	}
}


SYSCALL_HANDLER3(sys_open, int *fd_id, char *path , uint32_t flags) {
	int i=0;
	
	process_t* process = get_current_process();
	
	// recherche d une place dans la file descriptor table du process
	while (process->fd[i]) { 
		i++;
	}

	// ouverture du fichier
	if ((process->fd[i] = vfs_open(path, flags)) != NULL) {
		*fd_id = i;
	} else {
		*fd_id = -1;
	}
}

SYSCALL_HANDLER2(sys_close, int fd_id, uint32_t* ret)
{
	process_t * process = get_current_process();
	*ret = -1;
	open_file_descriptor *ofd;

	if (fd_id >= 0 && fd_id < FOPEN_MAX && process->fd[fd_id]) {
		ofd = process->fd[fd_id];
	
		if(ofd->f_ops->close == NULL)
			kerr("No \"close\" method for this device.");
		else
			*ret = ofd->f_ops->close(ofd);
		vfs_close(ofd);
		process->fd[fd_id] = NULL;
	}
}

SYSCALL_HANDLER3(sys_fcntl, int *fd_id, unsigned int request, void * data) {
	process_t * process = get_current_process();
	open_file_descriptor *ofd;
	if (process->fd[*fd_id]) {
		ofd = process->fd[*fd_id];

		if (request == F_SETFL) {
			ofd->flags = (int)data;
			*fd_id = 0;
			return;
		} else if (request == F_GETFL) {
			*fd_id = ofd->flags;
			return;
		}
	}
	*fd_id = -1;
}


