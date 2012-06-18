/**
 * @file vesa_types.h
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
 * Header file for the vesa driver.
 */

#ifndef _VESA_TYPES_H_
#define _VESA_TYPES_H_

#define _COLOR(_p, _bpp) _p.bpp ## _bpp
#define COLOR(_p, _bpp) _COLOR(_p, _bpp)

enum vesa_req_codes { SETMODE, GETVIDEOADDR, FLUSH };

struct vesa_setmode_req {
	int width;
	int height;
	int bpp;
};

typedef union {
	struct {
		char color;
	} bpp8;

	struct {
		unsigned b :5;
		unsigned g :5;
		unsigned r :5;
		unsigned   :1;
	} __attribute__ ((packed)) bpp15;

	struct {
		unsigned b :5;
		unsigned g :6;
		unsigned r :5;
	} __attribute__ ((packed)) bpp16;

	struct {
		unsigned char b;
		unsigned char g;
		unsigned char r;
	} bpp24;

	struct {
		unsigned char unused;
		unsigned char b;
		unsigned char g;
		unsigned char r;
	} bpp32;
} color_t;

#endif
