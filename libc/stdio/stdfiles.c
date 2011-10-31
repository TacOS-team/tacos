/**
 * @file stdfiles.c
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

/**
 * @file stdfiles.c
 */

#include <sys/types.h>
#include <libio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static FILE _IO_stdin = {_IO_MAGIC + _IO_LINE_BUF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, 0, 0};
static FILE _IO_stdout = {_IO_MAGIC + _IO_LINE_BUF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, &_IO_stdin, 1, 0};
static FILE _IO_stderr = {_IO_MAGIC + _IO_UNBUFFERED, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, &_IO_stdout, 2, 0};

FILE *stdin = & _IO_stdin;
FILE *stdout = & _IO_stdout;
FILE *stderr = & _IO_stderr;

FILE *__file_list = & _IO_stderr;
