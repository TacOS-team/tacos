/**
 * @file init.c
 *
 * @author TacOS developers 
 *
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


#include <symtable.h>
#include <stdio.h>
#include <klog.h>

int init(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused)))
{
	init_stdfiles();
	klog("Starting init process...");
	
	klog("Loading kernel symbol table...");
	load_kernel_symtable();
	
	
	/* Initialisation des drivers */
	klog("Initializing drivers...");
	init_driver_list();
	init_dummy();
	init_mouse();
	/* ************************** */
	
	klog("Starting user process...");
	exec_elf("/bin/mishell", 0);
	
	while(1);
	return 0;
}
