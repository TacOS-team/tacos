/**
 * @file vesa.c
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
 * VBE driver.
 *
 * @see http://wiki.osdev.org/Bochs_VBE_Extensions
 */
#include <stdio.h>
#include <fs/devfs.h>
#include <types.h>
#include <klog.h>
#include <ioports.h>
#include <string.h>
#include <pci.h>
#include <pci_config.h>
#include <memory.h>
#include <pagination.h>
#include <vmm.h>
#include <vesa_types.h>

#define VBE_DISPI_IOPORT_INDEX		0x01CE
#define VBE_DISPI_IOPORT_DATA		0x01CF
#define VBE_DISPI_INDEX_ID		0x0
#define VBE_DISPI_INDEX_XRES		0x1
#define VBE_DISPI_INDEX_YRES		0x2
#define VBE_DISPI_INDEX_BPP		0x3
#define VBE_DISPI_INDEX_ENABLE		0x4
#define VBE_DISPI_INDEX_BANK		0x5
#define VBE_DISPI_INDEX_VIRT_WIDTH	0x6
#define VBE_DISPI_INDEX_VIRT_HEIGHT	0x7
#define VBE_DISPI_INDEX_X_OFFSET	0x8
#define VBE_DISPI_INDEX_Y_OFFSET	0x9

#define VBE_DISPI_DISABLED		0x00
#define VBE_DISPI_ENABLED		0x01
#define VBE_DISPI_GETCAPS		0x02
#define VBE_DISPI_8BIT_DAC		0x20
#define VBE_DISPI_LFB_ENABLED		0x40
#define VBE_DISPI_NOCLEARMEM		0x80

#define V_BACKBUFFER_BASE		0x10000000

static int width;
static int height;
static int bpp;
static paddr_t p_lfb_base;
static paddr_t p_frontbuffer_base;
static paddr_t p_backbuffer_base;
static int doublebuf_enabled = 0;

static void vbe_write_reg(unsigned short index, unsigned short data) {
	outw(index, VBE_DISPI_IOPORT_INDEX);
	outw(data, VBE_DISPI_IOPORT_DATA);
}

static unsigned short vbe_read_reg(unsigned short index) __attribute__ ((unused));
static unsigned short vbe_read_reg(unsigned short index) {
	outw(index, VBE_DISPI_IOPORT_INDEX);
	return inw(VBE_DISPI_IOPORT_DATA);
}

static void remap_backbuffer(paddr_t p_addr_base) {
	size_t back_buffer_size = width * height * bpp;
	unsigned int page;
	for (page = 0; page < back_buffer_size/PAGE_SIZE; page++) {
		unmap(V_BACKBUFFER_BASE + page * PAGE_SIZE);
		map(p_addr_base + page * PAGE_SIZE, V_BACKBUFFER_BASE + page * PAGE_SIZE, 1);
	}
}

static int vesa_ioctl(open_file_descriptor* ofd __attribute__ ((unused)), unsigned int request, void* data) {
	struct vesa_setmode_req* req = (struct vesa_setmode_req*)data;
	switch (request) {
		case SETMODE:
			vbe_write_reg(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
			vbe_write_reg(VBE_DISPI_INDEX_XRES, req->width);
			vbe_write_reg(VBE_DISPI_INDEX_YRES, req->height);
			vbe_write_reg(VBE_DISPI_INDEX_BPP, req->bpp);
			vbe_write_reg(VBE_DISPI_INDEX_VIRT_WIDTH, req->width);
			vbe_write_reg(VBE_DISPI_INDEX_X_OFFSET, 0);
			vbe_write_reg(VBE_DISPI_INDEX_Y_OFFSET, 0);
			vbe_write_reg(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);

			width = req->width;
			height = req->height;
			bpp = (req->bpp + (8 - 1))/8;

			if (doublebuf_enabled) {  
				p_backbuffer_base = p_lfb_base;
				p_frontbuffer_base = p_lfb_base + width * height * bpp;

				while (p_frontbuffer_base % PAGE_SIZE != 0) {
					p_frontbuffer_base += width * bpp;
				}
			}

			return 0;
		case FLUSH:
			if (doublebuf_enabled) {
				// Décalage de la zone du LFB affichée
				vbe_write_reg(VBE_DISPI_INDEX_Y_OFFSET, (p_backbuffer_base - p_lfb_base) / (width * bpp));
				// Swap backbuffer/frontbuffer
				paddr_t tmp = p_backbuffer_base; 
				p_backbuffer_base = p_frontbuffer_base;
				p_frontbuffer_base = tmp;
				// Remap backbuffer
				remap_backbuffer(p_backbuffer_base);
				// Vidage backbuffer
				memset((void*)V_BACKBUFFER_BASE, 0, width * height * bpp);

				return 0;
			} else {
				return -1;
			}
		case GETVIDEOADDR:
			*((void**) data) = (void*) V_BACKBUFFER_BASE;
			return 0;
		default:
			return -1;
	}
} 

static chardev_interfaces di = {
	.read = NULL,
	.write = NULL,
	.open = NULL,
	.close = NULL,
	.ioctl = vesa_ioctl,
};

void init_vesa() {
	klog("initializing vesa driver...");

	if (register_chardev("vesa", &di) != 0) {
		kerr("error registering vesa driver.");
	}

	p_lfb_base = pci_read_value(pci_find_device(0x1234, 0x1111), PCI_BAR0) & 0xFFFF0000;
	klog("base addr : %x\n", p_lfb_base);

	size_t video_mem_size = 16 * 1024 * 1024; // 16Mio
	unsigned int page;
	for (page = 0; page < video_mem_size/PAGE_SIZE; page++) {
		map(p_lfb_base + page*PAGE_SIZE, V_BACKBUFFER_BASE + page*PAGE_SIZE, 1);
	}
}
