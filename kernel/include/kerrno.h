/**
 * @file kerrno.h
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
 * @brief Codes erreurs. 
 */

#ifndef _KERRNO_H
#define _KERRNO_H

#define EAGAIN    1 /**< Essai encore ;). */
#define EBADF     2 /**< Mauvais numero de fichier. */
#define EFAULT    3 /**< Mauvaise adresse. */
#define EFBIG     4 /**< Fichier trop gros. */
#define EINTR     5 /**< System call interrompu. */
#define EINVAL    6 /**< Argument invalide. */
#define EIO       7 /**< Erreur d'entrée / sortie. */
#define ENOSPC    8 /**< Plus d'espace libre. */
#define EPIPE     9 /**< broken pipe. */
#define ERANGE    10 /**< Résultat maths non representable. */
#define ENOENT    11 /**< entrée non trouvée. */
#define ENOTDIR	  12 /**< ce n'est pas un dossier. */
#define EEXIST    13 /**< existe déjà. */
#define EPERM     14 /**< operation non permise. */
#define EISDIR    15 /**< c'est un dossier. */
#define ENFILE    16 /**< Overflow de la file table. */
#define EMFILE    17 /**< Trop de fichiers ouverts. */

#endif
