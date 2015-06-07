/**
 * @file kdirent.h
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
 * @brief Syscall pour ce qui est relatif aux dossiers. 
 */

#ifndef _KDIRENT_H
#define _KDIRENT_H

#include <ksyscall.h>
#include <types.h>
#include <kstat.h>

#define NAME_MAX 256 /**< Longueur maximale d'un nom de fichier. */

/**
 * Entrée de dossier.
 */
struct dirent {
	uint32_t  d_ino; /**< Numéro inode. */
	uint16_t  d_reclen; /**< Longueur du nom de l'entrée. */
	uint8_t   d_type; /**< Type de fichier. */
	char      d_name[NAME_MAX]; /**< Nom de l'entrée. */
};

/**
 * Types de fichiers pour le champ "d_type".
 */
enum {
	DT_UNKNOWN = 0,
	DT_FIFO = 1,
	DT_CHR = 2,
	DT_DIR = 4,
	DT_BLK = 6,
	DT_REG = 8,
	DT_LNK = 10,
	DT_SOCK = 12,
	DT_WHT = 14
};

/**
 * Lecture d'un dossier.
 *
 * @param fd Identifiant du fichier ouvert.
 * @param entries Buffer qui contient des struct dirent.
 * @param size Taille du buffer.
 */
SYSCALL_HANDLER3(sys_readdir, int fd, char *entries, size_t *size);

/**
 * Création d'un dossier.
 *
 * @param pathname Nom du dossier.
 * @param mode Droits.
 * @param ret Valeur de retour.
 */
SYSCALL_HANDLER3(sys_mkdir, const char *pathname, mode_t mode, int *ret);

/**
 * Suppression d'un dossier.
 *
 * @param pathname Nom du dossier.
 * @param ret Valeur de retour.
 */
SYSCALL_HANDLER2(sys_rmdir, const char *pathname, int *ret);

#endif
