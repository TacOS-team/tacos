/**
 * @file mouse.c
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
 * Mouse driver.
 */

#include <kdriver.h>
#include <types.h>
#include <klog.h>
#include <ioports.h>
#include <interrupts.h>
#include <string.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

static int mouse_x = 0, mouse_y = 0;
static bool mouse_buttons[3] = { false };

inline void mouse_wait(int rw) { 
	int timeout = 100000;
	while ((timeout--) > 0) {
		// All output to port 0x60 or 0x64 must be preceded by waiting for bit 1 (value=2) of port 0x64 to become clear.
		// Similarly, bytes cannot be read from port 0x60 until bit 0 (value=1) of port 0x64 is set.
		if ((!rw && (inb(0x64) & 1) == 1) || (rw && (inb(0x64) & 2) == 0)) {
			timeout = 0;
		}
	}
}

inline unsigned char mouse_read() {
	mouse_wait(0);
	return inb(0x60);
}

inline void mouse_write(unsigned char val, unsigned int port) {
	mouse_wait(1);
	outb(val, port);
}

inline void mouse_write_command(unsigned char cmd) {
	mouse_write(0xD4, 0x64);
	mouse_write(cmd, 0x60);
}

void mouse_interrupt_handler() {
	static bool first = true;
	static int packet_num = 0;
	static int packet_x_sign, packet_y_sign;
	unsigned char packet;

	if (first) {
		first = false;
		return;
	}

	switch (packet_num) {
		case 0: // first packet: flags
			packet = inb(0x60);
			mouse_buttons[0] = packet & (0x01 << 0);
			mouse_buttons[1] = (packet & (0x01 << 1)) >> 1;
			mouse_buttons[2] = (packet & (0x01 << 2)) >> 2;
			packet_x_sign = packet & (0x01 << 4) ? 0xFFFFFF00 : 0 ; // sign extension for dx
			packet_y_sign = packet & (0x01 << 5) ? 0xFFFFFF00 : 0 ; // sign extension for dy
			packet_num++;
			break;
		case 1:	// delta X
			packet = inb(0x60);
			mouse_x += (packet_x_sign | packet);
			if (mouse_x >= SCREEN_WIDTH) {
				mouse_x = SCREEN_WIDTH - 1;
			}
			if (mouse_x < 0) {
				mouse_x = 0;
			}
			packet_num++;
			break;
		case 2: // delta Y
			packet = inb(0x60);
			mouse_y += (packet_y_sign | packet);
			if (mouse_y >= SCREEN_HEIGHT) {
				mouse_y = SCREEN_HEIGHT - 1;
			}
			if (mouse_y < 0) {
				mouse_y = 0;
			}
			packet_num = 0;
			break;
	}
}

void mouse_init() {
	mouse_x = SCREEN_WIDTH/2;
	mouse_y = SCREEN_HEIGHT/2;
	mouse_buttons[0] = mouse_buttons[1] = mouse_buttons[2] = false;

	// Get "compaq status" byte
	unsigned char status;
	mouse_write(0x20, 0x64);
	status = mouse_read();
	// Set bit 1 (Enable IRQ12) and clear bit 5 (Disable mouse clock)
	status = ((status | (1 << 1)) & ~(1 << 5));
	// Send back status byte
	mouse_write(0x60, 0x64);
	mouse_write(status, 0x60);
	
	// Use default options
	mouse_write_command(0xF6);
	mouse_read();

	// Enable packets
	mouse_write_command(0xF4);
	mouse_read();

	// Setup interrupt handler
	interrupt_set_routine(IRQ_PS2_MOUSE, mouse_interrupt_handler, 0);
}

size_t mouse_read_state(open_file_descriptor* ofd __attribute__((unused)), void* buf, size_t count __attribute__((unused))) {
	memcpy(buf, &mouse_x, sizeof(mouse_x));
	memcpy(buf + sizeof(mouse_x), &mouse_y, sizeof(mouse_y));
	memcpy(buf + sizeof(mouse_x) + sizeof(mouse_y), mouse_buttons, sizeof(mouse_buttons)); 
	return sizeof(mouse_x) + sizeof(mouse_y) + sizeof(mouse_buttons);
}

static driver_interfaces di = {
	.read = mouse_read_state,
	.write = NULL,
	.seek = NULL,
	.open = NULL,
	.close = NULL,
	.flush = NULL,
	.ioctl = NULL
};

void init_mouse() {
	klog("initializing mouse driver...");
	mouse_init();
	if (register_driver("mouse", &di) != 0) {
		kerr("error registering mouse driver.");
	}
}
