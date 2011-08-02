/**
 * @file kunistd.c
 *
 * @author TacOS developers 
 *
 * Maxime Cheramy <maxime81@gmail.com>
 * Nicolas Floquet <nicolasfloquet@gmail.com>
 * Benjamin Hautbois <bhautboi@gmail.com>
 * Ludovic Rigal <ludovic.rigal@gmail.com>
 * Simon Vernhes <simon@vernhes.eu>
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
#include <scheduler.h>
#include <types.h>
#include <libio.h>
#include <kfcntl.h>
#include <klog.h>
#include <stdio.h>

SYSCALL_HANDLER3(sys_write, uint32_t fd, const void *buf, size_t *c) {
	process_t * process = get_current_process();
	ssize_t *t = (ssize_t*)c;
	open_file_descriptor *ofd;

	if (process->fd[fd].used) {
		ofd = process->fd[fd].ofd;
		
		if(ofd->write == NULL)
		{
			kerr("No \"write\" method for this device.");
			*t = -1;
		}
		else
			*t = ofd->write(ofd, buf, *c);
	}
}

SYSCALL_HANDLER3(sys_read, uint32_t fd, const void *buf, size_t *c) {
	process_t * process = get_current_process();
	ssize_t *t = (ssize_t*)c;
	
	open_file_descriptor *ofd;

	if (process->fd[fd].used) {
		ofd = process->fd[fd].ofd;
		
		if(ofd->read == NULL)
		{
			kerr("No \"read\" method for this device.");
			*t = -1;
		}
		else
			*t = ofd->read(ofd, (void*) buf, *c);
	}
}

SYSCALL_HANDLER3(sys_seek, uint32_t fd, long *offset, int *whence) {
	process_t * process = get_current_process();

	open_file_descriptor *ofd;

	if (process->fd[fd].used) {
		ofd = process->fd[fd].ofd;
		
		if(ofd->seek == NULL)
			kerr("No \"seek\" method for this device.");
		else
			*whence = ofd->seek(ofd, *offset, *whence);
	}
}
