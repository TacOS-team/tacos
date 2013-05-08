/**
 * @file fd_types.h
 *
 * @author TacOS developers 
 *
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
 * Description de ce que fait le fichier
 */

#ifndef _FD_TYPES_H
#define _FD_TYPES_H

#define FOPEN_MAX 500 /**< Nombre max d'ouverture de fichier. */

#define SEEK_SET 0 /**< SEEK à une position donnée. */
#define SEEK_CUR 1 /**< SEEK relatif à la position actuelle. */
#define SEEK_END 2 /**< SEEK par rapport à la fin du fichier. */

struct _open_file_descriptor;
struct _fs_instance_t;
struct _dentry_t;

struct _open_file_operations_t {
	size_t (*write)(struct _open_file_descriptor *, const void*, size_t);
	size_t (*read)(struct _open_file_descriptor *,void*, size_t);
	int (*seek)(struct _open_file_descriptor *, long, int);
	int (*ioctl)(struct _open_file_descriptor*, unsigned int, void *);
	int (*open) (struct _open_file_descriptor*);
	int (*close) (struct _open_file_descriptor*);
	int (*readdir) (struct _open_file_descriptor*, char*, size_t);
} open_file_operations_t;

/**
 * Descripteur de fichier ouvert. À chaque fichier ouvert, une structure de ce
 * type est instanciée pour y stocker les informations sur le fichier et le FS.
 */
typedef struct _open_file_descriptor {
// A dégager ? Infos dans l'inode.
	uint32_t flags;
	int current_cluster; //XXX à supprimer (sock en dépend encore).
	uint8_t buffer[512]; //XXX à supprimer.
	uint32_t current_octet; // Utile pour f_pos ?
	uint32_t first_cluster;
	uint32_t file_size; 

	// Utile ? on peut l'avoir par mnt->instance
	struct _fs_instance_t *fs_instance;
	// Rajouter inode ?
	struct _dentry_t *dentry;
	struct _mounted_fs_t *mnt;
	struct _open_file_operations_t *f_ops;
	void * extra_data;
} open_file_descriptor;

#endif
