/**
 * @file dirent.h
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
 * @brief Définition de DIR et struct dirent.
 */

#ifndef _DIRENT_H_
#define _DIRENT_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

#include <sys/types.h>
#include <sys/stat.h>

#define NAME_MAX 256 /**< Taille maximale d'un nom de fichier. */

/**
 * Structure permettant de lire un dossier.
 */
typedef struct _DIR {
	int			fd; /**< File descriptor. */
	size_t	allocation; /**< Espace réservé au block. */
	size_t	size; /**< Données dans le block. */
	size_t	offset; /**< Position dans le block. */

	off_t		filepos; /**< Offset dans le dossier. */

	char		data[1]; /**< Block qui est en fait plus grand que ça. */
} DIR;

/**
 * Directory entry.
 */
struct dirent {
	uint32_t  d_ino; /**< Numéro inode. */
	uint16_t  d_reclen; /**< Taille de l'entrée. */
	uint8_t   d_type; /**< Type de fichier. */
	char      d_name[NAME_MAX]; /**< Nom du fichier. */
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
 * Crée un nouveau répertoire.
 *
 * @param pathname Le chemin du dossier à créer.
 * @param mode Les droits d'accès.
 *
 * @return 0 en cas de succès.
 */
int mkdir(const char *pathname, mode_t mode);

/**
 * Ouvre un dossier pour lister les fichiers à l'intérieur.
 *
 * @param dirname Le chemin du dossier à ouvrir.
 *
 * @return Une structure DIR permettant de lire le contenu du dossier. 
 */
DIR* opendir(const char* dirname);

/**
 * Lecture de la prochaine entrée du dossier.
 *
 * @param dirp La structure DIR initialisée par opendir.
 *
 * @return Un directory entry.
 */
struct dirent* readdir(DIR* dirp);

/**
 * Réinitialisation au début du dossier.
 *
 * @param dirp La structure DIR à reinitialiser au début du fichier.
 */
void rewinddir(DIR* dirp);

/**
 * Fermeture d'un dossier ouvert.
 *
 * @param dirp La structure DIR initialisée par opendir.
 *
 * @return 0 en cas de succès.
 */
int closedir(DIR* dirp);

__END_DECLS

#endif
