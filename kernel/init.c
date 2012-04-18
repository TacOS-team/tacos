/**
 * @file init.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012 - TacOS developers.
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

#include <elf.h>

/* LibC */
#include <stdlib.h>


int init(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused)))
{
	//init_stdfiles();
	klog("Starting init process...");
	
	klog("Loading kernel symbol table...");
	load_kernel_symtable();
	
	
	/* Initialisation des drivers */
	klog("Initializing drivers...");
	init_dummy();
	init_mouse();
	init_vga();
	/* ************************** */
	
	putenv("PWD=/");
	putenv("PATH=/tacos/bin");

	klog("Starting user process...");
	exec_elf("/tacos/bin/getty /dev/tty0", 0);

	get_current_process()->state = PROCSTATE_WAITING;
	while(1);
	return 0;
}
