/**
 * @file kdirent.c
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

#include <vfs.h>
#include <kdirent.h>
#include <kmalloc.h>
#include <klibc/string.h>
#include <scheduler.h>
#include <klog.h>

SYSCALL_HANDLER3(sys_readdir, int fd, char *entries, size_t *size) {
	process_t * process = get_current_process();
	ssize_t *t = (ssize_t*)size;
	
	open_file_descriptor *ofd;

	if (process->fd[fd] != NULL) {
		ofd = process->fd[fd];
		
		if(ofd->f_ops->readdir == NULL) {
			kerr("No \"readdir\" method for this device.");
			*t = 0;
		} else {
			*t = ofd->f_ops->readdir(ofd, entries, *size);
		}
	} else {
		*t = 0;
	}
}

SYSCALL_HANDLER3(sys_mkdir, const char *pathname, mode_t mode, int *ret) {
	*ret = vfs_mkdir(pathname, mode);
}

SYSCALL_HANDLER2(sys_rmdir, const char *pathname, int *ret) {
	*ret = vfs_rmdir(pathname);
}
