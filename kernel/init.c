/**
 * @file init.c
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
 * Description de ce que fait le fichier
 */

/* Kernel */
#include <klog.h>
#include <scheduler.h>
#include <symtable.h>
#include <drivers/dummy_driver.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <drivers/vesa.h>
#include <drivers/sock.h>
#include <kmalloc.h>

/* Includes des fs */
#include <fs/fat.h>
#include <fs/ext2.h>
#include <fs/procfs.h>

/* Includes des drivers */
#include <drivers/beeper.h>
#include <drivers/console.h>
#include <drivers/floppy.h>
#include <drivers/keyboard.h>
#include <drivers/serial.h>
#include <drivers/video.h>


#include <pci.h>


int init(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused)))
{
	//init_stdfiles();
	klog("Starting init process...");
	
	beeper_init();
	pci_scan();

	klog("Init FS...");
	fat_init();
	ext2_init();
	procfs_init();

	klog("Mount FS!");
	vfs_mount(NULL, "proc", "ProcFS");
	vfs_mount("/dev/fd0", "core", "FAT");
	vfs_mount("/dev/fd1", "tacos", "EXT2");
//	vfs_mount("/dev/ram", "init", "EXT2");


	klog("Loading kernel symbol table...");
	load_kernel_symtable();
	
	
	/* Initialisation des drivers */
	klog("Initializing drivers...");
	init_dummy();
	init_mouse();
	init_vga();
	init_vesa();
	init_sock();
	/* ************************** */
	
	char **environ = kmalloc(sizeof(char*) * 3);
	environ[0] = "PWD=/";
	environ[1] = "PATH=/tacos/bin";
	environ[2] = NULL;

	klog("Starting user process...");
	int ret;
	sys_exec("/tacos/bin/getty /dev/tty0", environ, &ret);

	get_current_process()->state = PROCSTATE_WAITING;
	while(1);
	return 0;
}
