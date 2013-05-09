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

#include <fs/devfs.h>
#include <types.h>
#include <klog.h>
#include <ioports.h>
#include <interrupts.h>
#include <klibc/string.h>
#include <drivers/mouse.h>

#define DEFAULT_RES_WIDTH 320
#define DEFAULT_RES_HEIGHT 200

static mousestate_t mouse_state;
static int res_width, res_height;
static int mouse_x, mouse_y;

static inline void mouse_wait(int rw) { 
	int timeout = 100000;
	while ((timeout--) > 0) {
		// All output to port 0x60 or 0x64 must be preceded by waiting for bit 1 (value=2) of port 0x64 to become clear.
		// Similarly, bytes cannot be read from port 0x60 until bit 0 (value=1) of port 0x64 is set.
		if ((!rw && (inb(0x64) & 1) == 1) || (rw && (inb(0x64) & 2) == 0)) {
			timeout = 0;
		}
	}
}

static inline unsigned char mouse_read() {
	mouse_wait(0);
	return inb(0x60);
}

static inline void mouse_write(unsigned char val, unsigned int port) {
	mouse_wait(1);
	outb(val, port);
}

static inline void mouse_write_command(unsigned char cmd) {
	mouse_write(0xD4, 0x64);
	mouse_write(cmd, 0x60);
}

static void mouse_interrupt_handler() {
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
			mouse_state.b1 = packet & (0x01 << 0);
			mouse_state.b2 = (packet & (0x01 << 1)) >> 1;
			mouse_state.b3 = (packet & (0x01 << 2)) >> 2;
			packet_x_sign = packet & (0x01 << 4) ? 0xFFFFFF00 : 0 ; // sign extension for dx
			packet_y_sign = packet & (0x01 << 5) ? 0xFFFFFF00 : 0 ; // sign extension for dy
			packet_num++;
			break;
		case 1:	// delta X
			packet = inb(0x60);
			mouse_x += (packet_x_sign | packet);
			if (mouse_x >= res_width) {
				mouse_x = res_width - 1;
			}
			if (mouse_x < 0) {
				mouse_x = 0;
			}
			mouse_state.x = mouse_x;
			packet_num++;
			break;
		case 2: // delta Y
			packet = inb(0x60);
			mouse_y += (packet_y_sign | packet);
			if (mouse_y >= res_height) {
				mouse_y = res_height - 1;
			}
			if (mouse_y < 0) {
				mouse_y = 0;
			}
			mouse_state.y = mouse_y;
			packet_num = 0;
			break;
	}
}

void mouse_read_state(mousestate_t* state) {
	memcpy(state, &mouse_state, sizeof(mouse_state));
}

static ssize_t mouse_read_state_ofd(open_file_descriptor* ofd __attribute__((unused)), void* buf, size_t count __attribute__((unused))) {
	mouse_read_state((mousestate_t*)buf);
	return sizeof(mouse_state);
}

void mouse_setres(int width, int height) {
	res_width = width;
	res_height = height;
	mouse_state.x = mouse_x = res_width/2;
	mouse_state.y = mouse_y = res_height/2;
}

static int mouse_ioctl(open_file_descriptor* ofd __attribute__ ((unused)), unsigned int request, void* data) {
	switch (request) {
		case SETRES: {
			struct mouse_setres_req* req = (struct mouse_setres_req*)data;
			mouse_setres(req->width, req->height);
			return 0;
		}
		default:
			return -1;
	}
} 

static chardev_interfaces di = {
	.read = mouse_read_state_ofd,
	.write = NULL,
	.open = NULL,
	.close = NULL,
	.ioctl = mouse_ioctl
};

static void mouse_init() {
	mouse_setres(DEFAULT_RES_WIDTH, DEFAULT_RES_HEIGHT);
	mouse_state.b1 = 0;
	mouse_state.b2 = 0;
	mouse_state.b3 = 0;
	mouse_state.b4 = 0;
	mouse_state.b5 = 0;
	mouse_state.b6 = 0;

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

void init_mouse() {
	klog("initializing mouse driver...");
	mouse_init();
	if (register_chardev("mouse", &di) != 0) {
		kerr("error registering mouse driver.");
	}
}
