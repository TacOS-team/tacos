/**
 * @file tty.h
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
 * @brief Terminal posix-like.
 */

#ifndef _TTY_H_
#define _TTY_H_

#include <types.h>
#include <termios_types.h>
#include <fd_types.h>

#define TIOCGWINSZ 3 /**< Request get window size. */
#define TIOCSCTTY 4 /**< Request set ctrl terminal. */

#define MAX_INPUT 255 /**< Taille du buffer où sont enregistrés les caractères prêt pour la lecture. */

#define TTY_DRIVER_TYPE_SYSTEM 1 /**< tty interne. */
#define TTY_DRIVER_TYPE_CONSOLE 2 /**< tty qui utilise l'une des consoles. */
#define TTY_DRIVER_TYPE_SERIAL 3 /**< tty qui utilise la liaison série. */
#define TTY_DRIVER_TYPE_PTY 4 /**< pseudo tty. */

struct _tty_driver_t;

/*  intr=^C   quit=^\   erase=bs kill=^U
  eof=^D    vtime=\0  vmin=\1   sxtc=\0
  start=^Q  stop=^S   susp=^Z   eol=\0
  reprint=^R  discard=^U  werase=^W lnext=^V
  eol2=\0
*/
#define INIT_C_CC {3, 28, 127, 21, 4, 0, 1, 0, 17, 19, 26, 0, 18, 21, 23, 22, 0}

extern struct termios tty_std_termios;

/**
 * Structure qui est passée à chaque appel de fonction (open, close, write...) au tty driver.
 */
typedef struct _tty_struct_t {
	int index; /**< Équivalent du minor number.*/
	struct termios termios; /**< Configuration du terminal. */
	char buffer[MAX_INPUT]; /**< Données prêtes pour la lecture. */
	unsigned int p_begin;
	unsigned int p_end;
	struct _tty_driver_t *driver;
	uint8_t sem;
	int fg_process;
	int n_open;
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

/**
 * Représente une taille de fenêtre.
 */
struct winsize {
	unsigned short ws_row; /**< Nombre de lignes. */
	unsigned short ws_col; /**< Nombre de colonnes. */
};

/**
 * Allocation d'un driver de terminal.
 *
 * @param lines Nombre de terminaux géré par ce driver.
 *
 * @return le driver alloué.
 */
tty_driver_t *alloc_tty_driver(int lines);

/**
 * Enregistre en tant que char device un driver alloué et configuré.
 *
 * @param driver Le driver à enregistrer.
 *
 * @return 0 en cas de succès (mais ne détecte pas encore les erreurs...).
 */
int tty_register_driver(tty_driver_t *driver);

/**
 * Ajoute un caractère dans le butter du tty.
 *
 * @param tty le tty qui nous intéresse.
 * @param ch le caractère à ajouter.
 */
void tty_insert_flip_char(tty_struct_t *tty, unsigned char ch);

/**
 * @brief Initialisation du système de terminaux.
 */
void tty_init();

#endif
