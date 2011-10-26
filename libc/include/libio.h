/**
 * @file libio.h
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

#ifndef _LIBIO_H_
#define _LIBIO_H_

/**
 * @file libio.h
 */

#include <types.h>

#define FOPEN_MAX 500

#define _IO_MAGIC 0xFBAD0000 /* Magic number (je le garde par compatibilité) */
#define _IO_MAGIC_MASK 0xFFFF0000 /* Pour filtrer le magic number */
#define _IO_UNBUFFERED 2 
#define _IO_LINE_BUF 0x200 
// TODO : définir d'autres flags (cf libc)

struct _open_file_descriptor;

typedef struct _file_descriptor {
	struct _open_file_descriptor *ofd;
	bool used;
} file_descriptor;

/**
 * @brief Gestion de stream. Inspiré de posix.
 * @struct _IO_FILE
 */
struct _IO_FILE {
	int _flags;		/* High-order word is _IO_MAGIC; rest is flags. */

	/* The following pointers correspond to the C++ streambuf protocol. */
	/* Note:  Tk uses the _IO_read_ptr and _IO_read_end fields directly. */
	char* _IO_read_ptr;		/**< Current read pointer */
	char* _IO_read_end;		/**< End of get area. */
	char* _IO_read_base;		/**< Start of putback+get area. */
	char* _IO_write_base;	/**< Start of put area. */
	char* _IO_write_ptr;		/**< Current put pointer. */
	char* _IO_write_end;		/**< End of put area. */
	char* _IO_buf_base;		/**< Start of reserve area. */
	char* _IO_buf_end;		/**< End of reserve area. */
	/* The following fields are used to support backing up and undo. */
	char *_IO_save_base;		/**< Pointer to start of non-current get area. */
	char *_IO_backup_base;  /**< Pointer to first valid character of backup area */
	char *_IO_save_end;		/**< Pointer to end of non-current get area. */

	struct _IO_FILE *_chain;

	int _fileno;
	int _flags2;
};

typedef struct _IO_FILE FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

#endif
