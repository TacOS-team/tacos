/**
 * @file vga.h
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
 * Header file for the VGA driver.
 */

#ifndef _VGA_H_
#define _VGA_H_

#include <vga_types.h>

void init_vga();
void vga_set_mode(enum vga_mode mode);
void vga_write_buf(char *buffer);

#endif
