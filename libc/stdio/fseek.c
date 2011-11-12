/**
 * @file fseek.c
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

#include <stdio.h>
#include <unistd.h>

int fseek(FILE *stream, long offset, int whence) {
	int b = 0;
	if (whence == SEEK_CUR && stream->_IO_read_base != NULL) {
		// On retire ce qu'on a bufferisé mais pas encore lu.
		b = (stream->_IO_read_end - stream->_IO_read_ptr);
	}
	if (whence != SEEK_CUR || offset >= b) {
		stream->_IO_read_end = stream->_IO_read_base;
		stream->_IO_read_ptr = stream->_IO_read_base;
		if (lseek(stream->_fileno, offset - b, whence)) {
			return 0;
		}
	} else {
		stream->_IO_read_ptr += offset;
		return 0;
	}
	return -1;
}

long ftell(FILE *stream) {
	int b = 0;
	if (stream->_IO_read_base != NULL) {
		// On retire ce qu'on a bufferisé mais pas encore lu.
		b = (stream->_IO_read_end - stream->_IO_read_ptr);
	}
	return lseek(stream->_fileno, 0, SEEK_CUR) - b;
}

void rewind(FILE *stream) {
	fseek(stream, 0L, SEEK_SET);
}


