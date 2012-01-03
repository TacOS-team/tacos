/**
 * @file fmemopen.c
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
#include <stdlib.h>
#include <libio.h>
#include "fopen.h"

int fmemopen_close(FILE *stream) {
	free(stream->_IO_buf_base);
	return 0;
}

/**
 *	Cette fonction associe un buffer de taille size à un nouveau stream.
 *	Les modes sont les même que fopen.
 */
FILE *fmemopen(void *buf, size_t size, const char *mode) {
//	int flags = convert_flags(mode);
	int pos_buf = 0;
	size_t taille_buf = 0;

	/* Si un pointeur null est donné pour le buffer, alors fmemopen doit aller size octets de mémoire */
	if (buf == NULL) {
		buf = malloc(size);
		/* Devrait failer si ce ne pas utilisé pour un update (mode contient "+") 
		 * (d'après la doc, encore pas très clair pour moi) */
	} else {

		/* Position : */
		/* La position dans le buffer est initialisée au début du buffer (pour les modes r et w) */
		if (mode[0] == 'r' || mode[0] == 'w') {
			pos_buf = 0;
		}

		/* La position dans le buffer est initialisée au premier octet null dans le buffer (pour les modes a) */
		/* La position dans le buffer est initialisée un octet après la fin du buffer si aucun octet null a été trouvé. */
		else if (mode[0] == 'a') {
			size_t pos = 0;
			while (pos < size && ((char*)buf)[pos] != '\0') {
				pos++;
			}
			pos_buf = pos;
		}

		/* Taille du buffer : */

		/* Pour les modes r et r+, la taille c'est size */
		if (mode[0] == 'r') {
			taille_buf = size;
		}
		/* Pour les modes w et w+, la taille c'est 0 */
		else if (mode[0] == 'w') {
			taille_buf = 0;
		}
		/* Pour les modes a et a+, la taille c'est la position du premier caractère null ou size si non trouvé. */
		else if (mode[0] == 'a') {
			size_t pos = 0;
			while(pos < size && ((char *) buf)[pos] != '\0');
			taille_buf = pos;
		}
	}
	/* TODO : lier fmemopen_close, fmemopen_read, fmemopen_write ! */

	FILE * stream = malloc(sizeof(FILE));
	stream->_fileno = -1;

	stream->_IO_buf_base = buf;
	stream->_IO_buf_end = buf + taille_buf; 
	stream->_IO_write_base = buf;
	stream->_IO_write_ptr = buf + pos_buf;
	stream->_IO_write_end = buf + taille_buf;
	stream->_flags = _IO_MAGIC + _IO_UNBUFFERED;

	return stream;
}
