/**
 * @file stdfiles.c
 *
 * @author TacOS developers 
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
 * @brief Définition et initialisation des fichiers standards (stdin, stdout,
 * stderr).
 */

#include <sys/types.h>
#include <libio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

static FILE _IO_stdin = {_IO_MAGIC + _IO_LINE_BUF + O_RDONLY, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, 0};
static FILE _IO_stdout = {_IO_MAGIC + _IO_LINE_BUF + O_WRONLY, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, 1};
static FILE _IO_stderr = {_IO_MAGIC + _IO_UNBUFFERED + O_WRONLY, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, 2};

FILE *stdin;
FILE *stdout;
FILE *stderr;

FILE *__file_list = NULL;

void init_stdfiles(void) {
	/* Allocation memoire. */
	stdin = (FILE*) malloc(sizeof(FILE));
	stdout = (FILE*) malloc(sizeof(FILE));
	stderr = (FILE*) malloc(sizeof(FILE));
	
	/* On y copie les valeurs par défaut */
	memcpy(stdin, &_IO_stdin, sizeof(FILE));
	memcpy(stdout, &_IO_stdout, sizeof(FILE));
	memcpy(stderr, &_IO_stderr, sizeof(FILE));

	stderr->_chain = stdout;
	stdout->_chain = stdin;
	stdin->_chain = NULL;
	__file_list = stderr;
}
