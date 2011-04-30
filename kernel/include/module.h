/**
 * @file module.h
 *
 * @author TacOS developers 
 *
 * Maxime Cheramy <maxime81@gmail.com>
 * Nicolas Floquet <nicolasfloquet@gmail.com>
 * Benjamin Hautbois <bhautboi@gmail.com>
 * Ludovic Rigal <ludovic.rigal@gmail.com>
 * Simon Vernhes <simon@vernhes.eu>
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
 * Loads & Unloads kernel modules
 */

#ifndef _MODULE_H
#define _MODULE_H

#include <types.h>

#define MODULE_NAME(X) char __modname[] __attribute__ ((section(".modname"))) = X
#define MODULE_VERSION(X) char __modversion[] __attribute__ ((section(".modversion"))) = X
#define MODULE_LOAD(X) int X(void) __attribute__ ((section(".modload")))
#define MODULE_UNLOAD(X) int X(void) __attribute__ ((section(".modunload")))

typedef struct {
	
	char* name;		/* Name loaded from .modname section */
	
	char* version; 	/* Version loaded from .modversion section */
	
	paddr_t load_handler; 	/* Address of the module_load function, loaded from .modload section */
	
	paddr_t unload_handler;	/* Address of the module_unload function, loaded from .modunload section */
	
	paddr_t load_addr;	/* Address where the module is loaded */
	
}module_info_t;

module_info_t* load_module(char* filename);

#endif /* _MODULE_H */
