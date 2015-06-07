/**
 * @file test_mouse.c
 *
 * @author TacOS developers 
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
 * Test program for the mouse driver:
 * displays a cursor and draws pixels when the left button is pressed,
 * clears them when the right button is pressed.
 */

#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vga_types.h>
#include <mouse_types.h>

char buffer[200][320];

int main() {
	int vga_fd = open("/dev/vga", O_RDONLY);
	ioctl(vga_fd, SETMODE, (void*)vga_mode_320x200x256);
	int mouse_fd = open("/dev/mouse", O_RDONLY);
	mousestate_t data;

	while (1) {
		read(mouse_fd, &data, sizeof(data));
		data.y = 199 - data.y;
		if (data.b1) {
			buffer[data.y][data.x] = 1;
		} else if (data.b2) {
			buffer[data.y][data.x] = 0;
		}
		char old = buffer[data.y][data.x];
		buffer[data.y][data.x] = 2;
		ioctl(vga_fd, FLUSH, buffer);
		buffer[data.y][data.x] = old;
		usleep(10000);
	}

	close(mouse_fd);
	close(vga_fd);

	return 0;
}
