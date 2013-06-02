/**
 * @file ram.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012, 2013 - TacOS developers.
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
 * Floppy driver
 */

#include <fs/devfs.h>
#include <kstdio.h>
#include <klibc/string.h>
#include <types.h>
#include <klog.h>

static vaddr_t start_addr;
static vaddr_t end_addr;

static int ram_open(open_file_descriptor *ofd);
static int ram_close(open_file_descriptor *ofd);
static ssize_t ram_read(open_file_descriptor *ofd, void* buf, size_t count, uint32_t offset);
static ssize_t ram_write(open_file_descriptor *ofd, const void* buf, size_t count, uint32_t offset);

static blkdev_interfaces di = { 
	.read = ram_read,
	.write = ram_write,
	.ioctl = NULL,
	.open = ram_open,
	.close = ram_close
};

int init_ramdisk(vaddr_t start, vaddr_t end) {
	start_addr = start;
	end_addr = end;
	register_blkdev("ram", &di);
	klog("Creating ramdisk: 0x%x -> 0x%x", start, end);
	return 0;
}

int ram_open(open_file_descriptor *ofd __attribute__((unused))) {
	klog("Opening ramdisk.");
	return 0;
}
int ram_close(open_file_descriptor *ofd __attribute__((unused))) {
	klog("Closing ramdisk.");
	return 0;
}

static ssize_t ram_read(open_file_descriptor *ofd __attribute__((unused)), void* buf, size_t count, uint32_t offset) {
	uint32_t real_offset = start_addr + offset;
	size_t real_count = 0;
	if(real_offset >= start_addr) {
		real_count = count;
		if(real_offset + count > end_addr) {
			/* On ne doit lire que ce qui est dans le ramdisk */
			real_count = end_addr - real_offset;
		}
		memcpy(buf, (void*) real_offset, real_count);
	}
	return real_count;
}

static ssize_t ram_write(open_file_descriptor *ofd __attribute__((unused)), const void* buf, size_t count, uint32_t offset) {
	uint32_t real_offset = start_addr + offset;
	size_t real_count = 0;
	if(real_offset >= start_addr) {
		real_count = count;
		if(real_offset + count > end_addr) {
			/* On ne doit pas ecrire hors du ramdisk */
			real_count = end_addr - real_offset;
		}
		memcpy((void*) real_offset, buf, real_count);
	}
	return real_count;
}
