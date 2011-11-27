/**
 * @file vga_types.h
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
 * Defines types used by graphic applications.
 */

#ifndef _VGA_TYPES_H_
#define _VGA_TYPES_H_

enum vga_mode { vga_mode_80x25_text, vga_mode_320x200x256, vga_mode_320x200x256_modex };

enum vga_req_codes { SETMODE, FLUSH, BACKTOTEXTMODE };

#endif
