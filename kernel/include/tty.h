/**
 * @file tty.h
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

#ifndef _TTY_H_
#define _TTY_H_

#include <types.h>
#include <termios.h>
#include <fd_types.h>

#define MAX_INPUT 255

#define TTY_DRIVER_TYPE_SYSTEM 1
#define TTY_DRIVER_TYPE_CONSOLE 2
#define TTY_DRIVER_TYPE_SERIAL 3
#define TTY_DRIVER_TYPE_PTY 4

struct _tty_driver_t;

struct termios tty_std_termios;

/**
 * Structure qui est passée à chaque appel de fonction (open, close, write...) au tty driver.
 */
typedef struct _tty_struct_t {
	int index; /**< Équivalent du minor number.*/
	void * driver_data; /**< De quoi enregistrer des données pour le driver.*/
	struct termios termios;
	char buffer[MAX_INPUT];
	unsigned int p_begin;
	unsigned int p_end;
	struct _tty_driver_t *driver;
	int sem;
} tty_struct_t;

/**
 *	Contient les opérations fournies par le tty driver.
 */
typedef struct _tty_operations_t {
	int (*open) (tty_struct_t*, open_file_descriptor*);
	int (*close) (tty_struct_t*, open_file_descriptor*);
	size_t (*write)(tty_struct_t*, open_file_descriptor*, const unsigned char*, size_t);
	void (*put_char)(tty_struct_t *tty, unsigned char ch);
	size_t (*set_termios)(struct termios*, struct termios*);
	int (*ioctl)(tty_struct_t *tty, open_file_descriptor*, unsigned int, void *); 
} tty_operations_t;


/**
 * Structure qui sert à l'enregistrement d'un driver tty.
 */
typedef struct _tty_driver_t {
	const char * driver_name; /**< Le nom du driver */
	const char * devfs_name; /**< Le prefixe à ajouter dans le devfs */
	int num; /**< Nombre de devices au maximum. */
	short type; /**< type : console, serial, pty */
	short subtype; /**< Soustype : ptm, pts par exemple */
	uint32_t flags; /**< Flags, tel que RESET_TERMIOS, REAL_RAW ou NO_DEVFS */
	struct termios init_termios;
	tty_struct_t **ttys;
	tty_operations_t *ops;
} tty_driver_t;


tty_driver_t *alloc_tty_driver(int lines);
void put_tty_driver(tty_driver_t *driver);
int tty_register_driver(tty_driver_t *driver);
void tty_insert_flip_char(tty_struct_t *tty, unsigned char ch);

void tty_init();

size_t tty_write(open_file_descriptor *ofd, const void *buf, size_t count);
size_t tty_read(open_file_descriptor *ofd, void *buf, size_t count);
int tty_ioctl (open_file_descriptor *ofd, unsigned int request, void *data);
int tty_close (open_file_descriptor *ofd);
int tty_open ();

#endif
