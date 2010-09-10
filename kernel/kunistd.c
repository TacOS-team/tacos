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
#include <types.h>
#include <libio.h>
#include <kfcntl.h>

void sys_write(uint32_t fd, uint32_t p_buf, uint32_t count) {
	process_t * process = get_current_process();
	const void *buf = (const void*)p_buf;
	size_t *c = (size_t*)count;
	ssize_t *t = (ssize_t*)count;

	open_file_descriptor *ofd;

	if (process->fd[fd].used) {
		ofd = process->fd[fd].ofd;

		*t = ofd->write(ofd, buf, *c);
	}
}

void sys_read(uint32_t fd, uint32_t p_buf, uint32_t count) {
	process_t * process = get_current_process();
	void *buf = (void*)p_buf;
	size_t *c = (size_t*)count;
	ssize_t *t = (ssize_t*)count;

	open_file_descriptor *ofd;

	if (process->fd[fd].used) {
		ofd = process->fd[fd].ofd;

		*t = ofd->read(ofd, buf, *c);
	}
}

void sys_seek(uint32_t fd, uint32_t p_offset, uint32_t p_whence) {
	process_t * process = get_current_process();
    long *offset = (long*)p_offset;
    int *whence = (int*)p_whence;

	open_file_descriptor *ofd;

	if (process->fd[fd].used) {
		ofd = process->fd[fd].ofd;

		*whence = ofd->seek(ofd, *offset, *whence);
	}
}
