/**
 * @file stat.h
 *
 * @author TacOS developers 
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
 */

#ifndef _STAT_H_
#define _STAT_H_

#include <sys/types.h>
#include <time.h>

/* File types.  */
#define	S_IFMT	 0170000	/* These bits determine file type.  */
#define S_IFSOCK 0140000 /* Socket.  */
#define S_IFLNK  0120000 /* Symbolic link.  */
#define S_IFREG  0100000 /* Regular file.  */
#define S_IFBLK  0060000 /* Block device.  */
#define S_IFDIR  0040000 /* Directory.  */
#define S_IFCHR  0020000 /* Character device.  */
#define S_IFIFO  0010000 /* FIFO.  */

#define S_ISBLK(m)  (((m) & S_IFMT) == S_IFBLK)
#define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR)
#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG)
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)

typedef uint32_t mode_t;
typedef uint32_t dev_t;
typedef uint32_t uid_t;
typedef uint32_t gid_t;
typedef uint32_t nlink_t;
typedef uint32_t blksize_t;
typedef uint32_t blkcnt_t;
typedef uint32_t ino_t;

struct stat {
	dev_t     st_dev;      /**< Périphérique                    */
	ino_t     st_ino;      /**< Numéro inœud                    */
	mode_t    st_mode;     /**< Protection + file type            */
	nlink_t   st_nlink;    /**< Nb liens matériels              */
	uid_t     st_uid;      /**< UID propriétaire                */
	gid_t     st_gid;      /**< GID propriétaire                */
	dev_t     st_rdev;     /**< Type périphérique               */
	off_t     st_size;     /**< Taille totale en octets         */
	blksize_t st_blksize;  /**< Taille de bloc pour E/S         */
	blkcnt_t  st_blocks;   /**< Nombre de blocs de 512B alloués */
	time_t    st_atime;    /**< Heure dernier accès             */
	time_t    st_mtime;    /**< Heure dernière modification     */
	time_t    st_ctime;    /**< Heure dernier changement état   */
};


#endif
