/**
 * @file kstdio.c
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
 * Description de ce que fait le fichier
 */

#define __KTW_COLUMNS 80
#define __KTW_LINES 25

#include <video.h>
#include <fcntl.h>
#include <kfcntl.h>
#include <stdlib.h>
#include <console.h>

/* Affichage des hexa en longueur fixe, c'est plus commode */
void itox(char *buf, int d)
{
	int i;
	int tmp = d;
	int digit;
	for(i=0; i<8; i++)
	{
		digit = (char)(tmp&0x0000000f);
		buf[7-i] = digit>9?'a'+(digit-10):'0'+(digit);
		tmp >>= 4;
	}
	buf[8] = '\0';
}

void kprintf(const char *format, ...) {
	static int n = -1;
	if (n == -1) {
		n = get_available_console(NULL);
	}

	char **arg = (char **) &format;
	int c;
	char buf[20];

	arg++;

	while ((c = *format++) != 0) {
		if (c != '%')
			kputchar(n, c);
		else {
			char *p;

			c = *format++;
			switch (c) {
			case 'd':
			case 'u':
				itoa(buf, c, *((int *) arg++));
				p = buf;
				goto string;
				break;
			case 'x':
				itox(buf,*((int *) arg++));
				p = buf;
				goto string;
				break;
			case 's':
				p = *arg++;
				if (!p)
					p = "(null)";

				string: while (*p)
					kputchar(n, *p++);
				break;

			default:
				kputchar(n, *((int *) arg++));
				break;
			}
		}
	}
}
