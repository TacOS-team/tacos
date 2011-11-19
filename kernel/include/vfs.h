/**
 * @file vfs.h
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
 * Description de ce que fait le fichier
 */

#ifndef _VFS_H_
#define _VFS_H_

#include <types.h>
#include <fd_types.h>
//XXX: libc
#include <sys/stat.h>

struct _fs_instance_t;

/**
 * @brief Structure qui représente un FS.
 */
typedef struct {
	char *name;	/**< Nom du FS, c'est aussi son "type" utilisé lors du mount. */
	//XXX: ajouter device en argument de mount.
	struct _fs_instance_t * (*mount) (); /**< Pointeur vers la fonction mount du FS. */
	void (*umount) (struct _fs_instance_t *); /**< Pointeur vers la fonction umount du FS. */
} file_system_t;

/**
 * @brief Instance d'un couple FS/Device monté.
 */
typedef struct _fs_instance_t {
	file_system_t *fs; /**< Pointeur vers le FS utilisé. */ //XXX: Est-ce utile ?
	// XXX: enregistrer @device ?
	int (*open) (struct _fs_instance_t *, const char * , open_file_descriptor *, uint32_t);
	int (*opendir) (struct _fs_instance_t *, const char *);
	int (*readdir) (struct _fs_instance_t *, const char *, int, char *);
	int (*mkdir) (struct _fs_instance_t *, const char * , mode_t);
	int (*stat) (struct _fs_instance_t *, const char *, struct stat *);
} fs_instance_t;

/**
 * @brief Enregistrer un FS dans le VFS pour le rendre disponible.
 */
void vfs_register_fs(file_system_t *fs);

int vfs_open(const char * pathname, open_file_descriptor *ofd, uint32_t modes);
void vfs_mount(const char *device, const char *mountpoint, const char *type);
int vfs_umount(const char *mountpoint);
int vfs_mkdir(const char * pathname, mode_t mode);
int vfs_opendir(const char *path);
int vfs_readdir(const char * pathname, int iter, char * filename);
int vfs_stat(const char *pathname, struct stat *);

#endif
