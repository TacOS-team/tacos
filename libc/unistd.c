/**
 * @file unistd.c
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

#include <unistd.h>
#include <stdio.h>
#include <syscall.h>

unsigned int sleep(unsigned int seconds)
{
	return usleep(1000000*seconds);
}

unsigned int usleep(unsigned int microseconds)
{
	syscall(SYS_SLEEP, microseconds, 0, 0);
	return 0;
}

ssize_t write(int fd, const void *buf, size_t count) {
	syscall(SYS_WRITE, fd, (uint32_t) buf, (uint32_t)(&count));

	return count;
}

ssize_t read(int fd, void *buf, size_t count) {
	syscall(SYS_READ, fd, (uint32_t) buf, (uint32_t)(&count));

	return count;
}

int seek(int fd, long offset, int whence) {
	syscall(SYS_SEEK, fd, (uint32_t)(&offset), (uint32_t)(&whence));

	return whence;
}
