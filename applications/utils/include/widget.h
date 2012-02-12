/**
 * @file widget.h
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

#ifndef WIDGET_H
#define WIDGET_H

#define MAX_WIDGETS 100

enum widget_type
{
	BUTTON,
	TXT
};

struct window_t;

struct widget_t
{
	char visible;
	char* adv;
	enum widget_type type;
	unsigned char x;
	unsigned char y;
	unsigned char w;
	unsigned char h;
	unsigned char bg;
	unsigned char fg;
	struct window_t* father;
	void (*onClick)(struct widget_t*, int, int);
};

struct window_t
{
	int nb_widgets;
	unsigned char bg;
	unsigned char cursor;
	struct widget_t* widgets[MAX_WIDGETS];
};


#endif //WIDGET_H

