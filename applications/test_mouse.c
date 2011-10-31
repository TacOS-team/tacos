/**
 * @file test_mouse.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2011 - TacOS developers.
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
 * Test program for the mouse driver:
 * displays a cursor and draws pixels when the left button is pressed,
 * clears them when the right button is pressed.
 */

#include <fcntl.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <vga_types.h>

char buffer[200][320];

struct mousedata {
	int x;
	int y;
	bool buttons[3];
};

int main() {
	int fd = open("$mouse", O_RDONLY);
	struct mousedata data;
	syscall(SYS_VGASETMODE, vga_mode_320x200x256, 0, 0);

	while (1) {
		read(fd, &data, sizeof(data));
		data.y = 200 - data.y;
		if (data.buttons[0]) {
			buffer[data.y][data.x] = 1;
		} else if (data.buttons[1]) {
			buffer[data.y][data.x] = 0;
		}
		char old = buffer[data.y][data.x];
		buffer[data.y][data.x] = 2;
		syscall(SYS_VGAWRITEBUF, (uint32_t)buffer, 0, 0);
		buffer[data.y][data.x] = old;
		usleep(10000);
	}
	close(fd);
	return 0;
}
