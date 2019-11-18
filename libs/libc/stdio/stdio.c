/**
 * @file stdio.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2015 TacOS developers.
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
#include <stdlib.h>

int setvbuf(FILE *stream, char *buf, int mode, size_t size) {
  //kfree(stream->buffer);
	if(buf == NULL) {
		size = (mode == _IONBF) ? 1 : 512;
		stream->_IO_buf_base = malloc(size);
	} else {
		stream->_IO_buf_base = buf;
	}
	stream->_IO_buf_end = stream->_IO_buf_base + size;

	stream->_IO_read_base = stream->_IO_buf_base;
	stream->_IO_read_end = stream->_IO_buf_base;
	stream->_IO_read_ptr = stream->_IO_buf_base;
	stream->_IO_write_base = stream->_IO_buf_base;
	stream->_IO_write_end = stream->_IO_buf_base;
	stream->_IO_write_ptr = stream->_IO_buf_base;

	stream->_flags = mode;

	return 0;
}

