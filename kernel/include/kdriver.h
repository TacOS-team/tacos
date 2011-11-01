/**
 * @file kdriver.h
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

#ifndef _KDRIVER_H
#define _KDRIVER_H

#include <types.h>
#include <kfcntl.h>

typedef struct {
	size_t (*read)(open_file_descriptor *,void*, size_t);
	size_t (*write)(open_file_descriptor *, const void*, size_t);
	int (*seek) (open_file_descriptor *, long, int);
	int (*ioctl) (open_file_descriptor*, unsigned int, unsigned long);
	int (*open) (open_file_descriptor*);
	int (*close) (open_file_descriptor*);
	int (*flush) (open_file_descriptor*);
}driver_interfaces;

void init_driver_list();
driver_interfaces* find_driver(char* name);
int register_driver(const char* name, driver_interfaces* di);

#endif /* _KDRIVER_H */
