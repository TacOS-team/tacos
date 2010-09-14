/**
 * @file get.c
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
// TODO : Réécriture complète de la fonction.
// La gestion du backspace doit se faire au niveau supérieur (il n'y a que dans une "console" qu'on se sert du backspace). À voir comment gérer correctement la chose lors d'un scanf...
// Je ne suis pas sûr que ça soit normal d'avoir un flush ici...

	int c = EOF;
	char *i;
	
	char * pointeur;
	pointeur = stream->_IO_read_base;

	if (stream->_IO_read_ptr == NULL) {
      char * buf = (char*) malloc(1000);
      stream->_IO_buf_base = buf;
      stream->_IO_buf_end = buf + 1000;
      stream->_IO_read_base = buf;
      stream->_IO_read_end = buf;
      stream->_IO_read_ptr = buf;
	}

	if (stream->_IO_read_base == stream->_IO_read_ptr) {
		if (read(stream->_fileno, stream->_IO_read_ptr, 1) > 0) {
			stream->_IO_read_ptr++;
		} else {
			return EOF;
		}
	}
	
	c = *stream->_IO_read_base & 0xFF;
	for (i = stream->_IO_read_base; i < stream->_IO_read_ptr; i++) {
		*i = *(i+1);
	}

	stream->_IO_read_ptr--;
	return c;
}
/**
 * Fonction de la libc.
 * getchar est équivalent à fgetc(stdin).
 */
int getchar(void) {
	return fgetc(stdin);
}
