/**
 * @file stat.h
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
 */

#ifndef _STAT_H_
#define _STAT_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

#include <sys/types.h>
#include <time.h>

/* File types.  */
#define	S_IFMT	 0170000 /**< These bits determine file type.  */
#define S_IFSOCK 0140000 /**< Socket.  */
#define S_IFLNK  0120000 /**< Symbolic link.  */
#define S_IFREG  0100000 /**< Regular file.  */
#define S_IFBLK  0060000 /**< Block device.  */
#define S_IFDIR  0040000 /**< Directory.  */
#define S_IFCHR  0020000 /**< Character device.  */
#define S_IFIFO  0010000 /**< FIFO.  */

#define S_ISBLK(m)  (((m) & S_IFMT) == S_IFBLK) /**< Is block device special file. */
#define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR) /**< Is char device special file. */
#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR) /**< Is a directory. */
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO) /**< Is a FIFO. */
#define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK) /**< Is a link. */
#define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG) /**< Is a regular file. */
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK) /**< Is a socket. */

#define S_IXOTH 001   /**< Execute by other. */
#define S_IROTH 002   /**< Read by other. */
#define S_IWOTH 004   /**< Write by other. */
#define S_IXGRP 0010  /**< Execute by group. */
#define S_IRGRP 0020  /**< Read by group. */
#define S_IWGRP 0040  /**< Write by group. */
#define S_IXUSR 00100 /**< Execute by user. */
#define S_IRUSR 00200 /**< Read by user. */
#define S_IWUSR 00400 /**< Write by user. */

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
	mode_t    st_mode;     /**< Protection + file type          */
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

__END_DECLS

#endif
