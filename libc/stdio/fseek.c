//      TACOS: fseek.c
//      
//      Copyright 2010 Nicolas Floquet <nicolasfloquet@gmail.com>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.


#include <stdio.h>

/* TODO Enlever les attributes quand la fonction sera codée... */
int fseek(FILE *stream, long offset, int whence)
{
	return seek(stream->_fileno, offset, whence);
}

int ftell(FILE *stream)
{
// Est-ce que ça marche ?
	return (int)(stream->_IO_read_ptr - stream->_IO_read_end);
}

void rewind(FILE *stream)
{
	fseek(stream, 0L, SEEK_SET);
}


