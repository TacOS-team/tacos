/**
 * @file get.c
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @file get.c
 */

char *fgets(char *s, int size, FILE *stream) {
	int i = 0;
	int c = 0;
	while (i < size - 1 && c != '\n') {
		c = fgetc(stream);
		if (c == EOF) { // Je pars du principe que le \n on le stock dans le buffer mais pas EOF.
			break;
		}
		s[i] = (char)c;
		i++;
	}
	s[i] = '\0'; // Termine la chaine par un \0.
	return s;
}

int fgetc(FILE *stream) {
	int c = EOF;
	ssize_t s;
	
	// Première utilisation du stream, on alloue un buffer.
	if (stream->_IO_buf_base == NULL) {
      char * buf = (char*) malloc(1024);
      stream->_IO_buf_base = buf;
      stream->_IO_buf_end = buf + 1024;
      stream->_IO_read_base = buf;
      stream->_IO_read_end = buf;
      stream->_IO_read_ptr = buf;
	}

	// Si on a plus d'octet dans le buffer de lecture, on rerempli le buffer.
	if (stream->_IO_read_ptr == stream->_IO_read_end) {
		if ((s = read(stream->_fileno, stream->_IO_read_base, 1024)) > 0) {
			stream->_IO_read_end = stream->_IO_read_base + s;
			stream->_IO_read_ptr = stream->_IO_read_base;
		} else {
			return EOF;
		}
	}

	// On prend l'octet le plus ancien dans le buffer.
	c = (unsigned char) *stream->_IO_read_ptr;

	// Il y a un élément en moins dans le buffer.
	stream->_IO_read_ptr++;

	return c;
}

/**
 * Fonction de la libc.
 * getchar est équivalent à fgetc(stdin).
 */
int getchar(void) {
	return fgetc(stdin);
}
