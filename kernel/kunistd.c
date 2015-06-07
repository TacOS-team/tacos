/**
 * @file kunistd.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2015 TacOS developers.
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
#include <ksem.h>
#include <kunistd.h>
#include <scheduler.h>
#include <vfs.h>

SYSCALL_HANDLER3(sys_write, uint32_t fd, const void *buf, size_t *c) {
	process_t * process = get_current_process();
	ssize_t *t = (ssize_t*)c;
	open_file_descriptor *ofd;

	if (process->fd[fd]) {
		ofd = process->fd[fd];
		
		if(ofd->f_ops->write == NULL) {
			kerr("No \"write\" method for this device.");
			*t = -1;
		} else {
			*t = ofd->f_ops->write(ofd, buf, *c);
		}
	} else {
		*t = -1;
	}
}

SYSCALL_HANDLER3(sys_read, uint32_t fd, const void *buf, size_t *c) {
	ssize_t *t = (ssize_t*)c;
	if (buf == NULL) {
		kerr("buffer null !");
		*t = -1;
		return;
	}

	process_t * process = get_current_process();
	
	open_file_descriptor *ofd;

	if (process->fd[fd]) {
		ofd = process->fd[fd];
		
		if(ofd->f_ops->read == NULL) {
			kerr("No \"read\" method for this device.");
			*t = -1;
		} else {
			*t = ofd->f_ops->read(ofd, (void*) buf, *c);
		}
	} else {
		*t = -1;
	}
}

SYSCALL_HANDLER3(sys_seek, uint32_t fd, long *offset, int whence) {
	process_t * process = get_current_process();

	open_file_descriptor *ofd;

	if (process->fd[fd]) {
		ofd = process->fd[fd];
		
		if(ofd->f_ops->seek == NULL) {
			kerr("No \"seek\" method for this device.");
			*offset = -1;
		} else {
			ofd->f_ops->seek(ofd, *offset, whence);
			*offset = ofd->current_octet;
		}
	}
}

SYSCALL_HANDLER3(sys_ioctl, uint32_t *fd, unsigned int request, void *data) {
	process_t * process = get_current_process();

	open_file_descriptor *ofd;

	if (process->fd[*fd]) {
		ofd = process->fd[*fd];

		if (ofd->f_ops->ioctl == NULL) {
			kerr("No \"ioctl\" method for this device.");
			*fd = -1;
		}
		else {
			*fd = ofd->f_ops->ioctl(ofd, request, data);
		}
	}
}

SYSCALL_HANDLER3(sys_select, uint32_t *fds, size_t n, int *ret) {
	process_t * process = get_current_process();
	int val = 0;
	uint8_t sem = ksemget(SEM_NEW, SEM_CREATE);
	ksemctl(sem, SEM_SET, &val);

	size_t i, j;
	open_file_descriptor *ofd;
	for (i = 0; i < n; i++) {
		ofd = process->fd[fds[i]];
		ofd->select_sem = sem;
		if (ofd->current_octet < ofd->inode->i_size) {
			// Found, exit
			ksemctl(sem, SEM_DEL, NULL); 
			for (j = 0; j <= i; j++) {
				ofd->select_sem = 0;
			}
			*ret = fds[i];
			return;
		}
	}

	ksemP(sem);

	for (i = 0; i < n; i++) {
		ofd = process->fd[fds[i]];
		ofd->select_sem = 0;
		if (ofd->current_octet < ofd->inode->i_size) {
			*ret = fds[i];
		}
	}

	ksemctl(sem, SEM_DEL, NULL);
}


SYSCALL_HANDLER2(sys_chdir, const char *path, int *ret) {
    *ret = vfs_chdir(path);
}

SYSCALL_HANDLER3(sys_readlink, const char *path, char *buf, ssize_t *ret) {
	*ret = vfs_readlink(path, buf, (size_t)*ret);
}

SYSCALL_HANDLER3(sys_stat, const char *path, struct stat *buf, int *ret) {
	*ret = vfs_stat(path, buf, *ret);
}

SYSCALL_HANDLER3(sys_symlink, const char *target, const char *linkpath, int *ret) {
	*ret = vfs_symlink(target, linkpath);
}

SYSCALL_HANDLER2(sys_unlink, const char *path, int *ret) {
	*ret = vfs_unlink(path);
}

SYSCALL_HANDLER3(sys_chmod, const char *path, mode_t mode, int *ret) {
	*ret = vfs_chmod(path, mode);
}

SYSCALL_HANDLER3(sys_utimes, const char *path, const struct timeval times[2], int *ret) {
	*ret = vfs_utimes(path, times);
}

SYSCALL_HANDLER3(sys_chown, const char *path, uid_t owner, gid_t *group) {
	*group = vfs_chown(path, owner, *group);
}

SYSCALL_HANDLER3(sys_rename, const char *oldpath, const char *newpath, int *ret) {
	*ret = vfs_rename(oldpath, newpath);
}

SYSCALL_HANDLER2(sys_dup, int oldfd, int *ret) {
	int i = 0;
	process_t* process = get_current_process();

	// recherche d'une place dans la file descriptor table du process :
	while(process->fd[i]) {
		i++;
	}

	process->fd[i] = process->fd[oldfd];

	*ret = i;
}

SYSCALL_HANDLER2(sys_dup2, int oldfd, int *newfd) {
	process_t* process = get_current_process();

	if (process->fd[oldfd]) {
		if (process->fd[*newfd]) {
			if (process->fd[*newfd]->f_ops->close != NULL) {
				process->fd[*newfd]->f_ops->close(process->fd[*newfd]);
			}
		}

		process->fd[*newfd] = process->fd[oldfd];

	} else {
		*newfd = -1;
	}
}


SYSCALL_HANDLER3(sys_mknod, const char *path, mode_t mode, dev_t *dev) {
	*dev = vfs_mknod(path, mode, *dev);
}
