/**
 * @file init.c
 *
 * @author TacOS developers 
 *
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
#include <kfcntl.h>
#include <kprocess.h>

/* Includes des fs */
#include <fs/devfs.h>
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

/**
 * Init stage 1
 *
 * Premier niveau d'initialisation, doit comprendre le minimum 
 * pour permettre le chargement de module. Le but ici est d'avoirs
 * tout le necessaire pour acceder a l'init ramdisk. On ne peut 
 * donc pas encore utiliser de modules.
 *
 */
static void init_stage_1(){
	klog("----------------------------");
	klog("---     Init Stage 1     ---");
	klog("----------------------------");
	
	vfs_init();
	/* Un fois que devfs est initialise, on peut creer tous les drivers */
	devfs_init();
	vfs_mount(NULL, "dev", "DevFS");
	//create_partition_service();
	klog("Init modules...");
	
	pci_scan();

	console_init();
	serial_init();

	process_t *new_proc = get_current_process();
	new_proc->ctrl_tty = "/dev/tty0";

	beeper_init();
	fat_init();
	ext2_init();
	procfs_init();
	init_dummy();
	init_mouse();
	init_vga();

	init_vesa();
	init_sock();
	init_floppy();

	klog("Mounting File Systems...");
	vfs_mount(NULL, "proc", "ProcFS");
	vfs_mount("/dev/fd0", "core", "FAT");
	vfs_mount("/dev/fd1", "tacos", "EXT2");
//	vfs_mount("/dev/ram", "init", "EXT2");

	klog("Loading kernel symbol table...");
	load_kernel_symtable();
	/* ************************** */
	
}

/**
 * Init stage 2
 *
 * Comprend tout ce qui peut etre fait une fois que le ramdisk
 * est disponible. On devrait commencer a utiliser des modules.
 *
 */
static void init_stage_2() {
	klog("----------------------------");
	klog("---     Init Stage 2     ---");
	klog("----------------------------");
}

/**
 * Init stage 3
 * 
 * Comprend tout ce qui peut etre fait apres le stage 2
 *
 */
static void init_stage_3() {
	klog("----------------------------");
	klog("---     Init Stage 3     ---");
	klog("----------------------------");
}

int init(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused)))
{
	//init_stdfiles();
	klog("Starting init process...");
	init_stage_1();
	init_stage_2();
	init_stage_3();	

	char **environ = kmalloc(sizeof(char*) * 3);
	environ[0] = "PWD=/";
	environ[1] = "PATH=/tacos/bin";
	environ[2] = NULL;

	klog("Starting user process...");
	int ret;
	sys_exec("/tacos/bin/getty /dev/tty0", environ, &ret);
	
	// Acknowledge the death of all its children.
	while (waitpid(-1) > -1);

	klog("bye.");
	return 0;
}
