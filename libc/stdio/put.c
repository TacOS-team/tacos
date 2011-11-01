/**
 * @file put.c
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int fflush(FILE *stream) {
	if (stream->_fileno > 0) {
		write(stream->_fileno, stream->_IO_write_base, stream->_IO_write_ptr - stream->_IO_write_base);
		stream->_IO_write_ptr = stream->_IO_write_base;
		stream->_IO_read_ptr = stream->_IO_read_base;
		stream->_IO_read_end = stream->_IO_read_base;
		return 0;
	} else {
		stream->_IO_read_ptr = stream->_IO_read_base;
		stream->_IO_read_end = stream->_IO_read_base;
		return 0;
	}
	return EOF;
}

int fputc(int c, FILE *stream) {
	// Ajoute dans le buffer.
	
	if (stream->_IO_buf_base == NULL) {
		// Si aucun buffer alors on en crée un nouveau.
		char * buf = malloc(1024);
		stream->_IO_buf_base = buf;
		stream->_IO_buf_end = buf + 1024;
		stream->_IO_write_base = stream->_IO_buf_base;
		stream->_IO_write_end = stream->_IO_buf_base;
		stream->_IO_write_ptr = stream->_IO_buf_base;
	}

	*(stream->_IO_write_ptr++) = c;

	// Consulte le comportement du buffer pour savoir s'il faut flusher :
	if (!(stream->_flags & (_IO_UNBUFFERED | _IO_LINE_BUF))) {
		// Fully buffered : on flush que si le buffer est full !

		// C'est full ?
		if (stream->_IO_write_ptr >= stream->_IO_buf_end) {
			fflush(stream);
		}
	} else if (stream->_flags & _IO_LINE_BUF) {
		// Line buffered : on flush si on prend un \n ou si on arrive à la 
		// fin du buffer.

		// Il faut flusher ?
		if (stream->_IO_write_ptr >= stream->_IO_buf_end || c == '\n') {
			fflush(stream);
		}
	} else {
		if (stream->_fileno == -1) {
			if (stream->_IO_write_ptr >= stream->_IO_buf_end) {
				return EOF;
			}
		} else {
			// Non bufferisé.
			fflush(stream);
		}
	}
	
	return c;
}

int fputs(const char *s, FILE *stream) {
	while(*s != '\0') {
		if(fputc(*s++, stream) == EOF) {
			return EOF;	
		}
	}
	return -2;
}

int putchar(int c) {
	return fputc(c, stdout);
}

int puts(const char *s) {
	return fputs(s, stdout);
}
