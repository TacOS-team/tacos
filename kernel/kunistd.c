/**
 * @file kunistd.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012 - TacOS developers.
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
#include <kprocess.h>
#include <kunistd.h>
#include <scheduler.h>
#include <vfs.h>

SYSCALL_HANDLER3(sys_write, uint32_t fd, const void *buf, size_t *c) {
	process_t * process = get_current_process();
	ssize_t *t = (ssize_t*)c;
	open_file_descriptor *ofd;

	if (process->fd[fd].used) {
		ofd = process->fd[fd].ofd;
		
		if(ofd->write == NULL) {
			kerr("No \"write\" method for this device.");
			*t = -1;
		} else {
			*t = ofd->write(ofd, buf, *c);
		}
	} else {
		*t = -1;
	}
}

SYSCALL_HANDLER3(sys_read, uint32_t fd, const void *buf, size_t *c) {
	process_t * process = get_current_process();
	ssize_t *t = (ssize_t*)c;
	
	open_file_descriptor *ofd;

	if (process->fd[fd].used) {
		ofd = process->fd[fd].ofd;
		
		if(ofd->read == NULL) {
			kerr("No \"read\" method for this device.");
			*t = -1;
		} else {
			*t = ofd->read(ofd, (void*) buf, *c);
		}
	} else {
		*t = -1;
	}
}

SYSCALL_HANDLER3(sys_seek, uint32_t fd, long *offset, int whence) {
	process_t * process = get_current_process();

	open_file_descriptor *ofd;

	if (process->fd[fd].used) {
		ofd = process->fd[fd].ofd;
		
		if(ofd->seek == NULL) {
			kerr("No \"seek\" method for this device.");
			*offset = -1;
		} else {
			ofd->seek(ofd, *offset, whence);
			*offset = ofd->current_octet;
		}
	}
}

SYSCALL_HANDLER3(sys_ioctl, uint32_t *fd, unsigned int request, void *data) {
	process_t * process = get_current_process();

	open_file_descriptor *ofd;

	if (process->fd[*fd].used) {
		ofd = process->fd[*fd].ofd;

		if (ofd->ioctl == NULL) {
			kerr("No \"ioctl\" method for this device.");
			*fd = -1;
		}
		else {
			*fd = ofd->ioctl(ofd, request, data);
		}
	}
}

SYSCALL_HANDLER3(sys_stat, const char *path, struct stat *buf, int *ret) {
	*ret = vfs_stat(path, buf);
}

SYSCALL_HANDLER2(sys_unlink, const char *path, int *ret) {
	*ret = vfs_unlink(path);
}

SYSCALL_HANDLER2(sys_dup, int oldfd, int *ret) {
	int i = 0;
	process_t* process = get_current_process();

	// recherche d'une place dans la file descriptor table du process :
	while(process->fd[i].used) {
		i++;
	}

	process->fd[i].used = true;
	process->fd[i].ofd = process->fd[oldfd].ofd;

	*ret = i;
}

SYSCALL_HANDLER2(sys_dup2, int oldfd, int *newfd) {
	process_t* process = get_current_process();

	if (process->fd[oldfd].used) {
		if (process->fd[*newfd].used) {
			if (process->fd[*newfd].ofd->close != NULL) {
				process->fd[*newfd].ofd->close(process->fd[*newfd].ofd);
			}
		}

		process->fd[*newfd].used = true;
		process->fd[*newfd].ofd = process->fd[oldfd].ofd;

	} else {
		*newfd = -1;
	}
}



SYSCALL_HANDLER3(sys_mknod, const char *path, mode_t mode, dev_t *dev) {
	*dev = vfs_mknod(path, mode, *dev);
}
