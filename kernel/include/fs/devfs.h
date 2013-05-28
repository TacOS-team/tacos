/**
 * @file fs/devfs.h
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
 * Description de ce que fait le fichier
 */

#ifndef _DEVFS_H
#define _DEVFS_H

#include <types.h>
#include <fd_types.h>

/**
 * Type de device.
 */
typedef enum _device_type_t{
	CHARDEV,
	BLKDEV
} device_type_t;

typedef ssize_t (*chardev_read_t)(open_file_descriptor *,void*, size_t);
typedef ssize_t (*chardev_write_t)(open_file_descriptor *, const void*, size_t);
typedef int (*chardev_ioctl_t) (open_file_descriptor*, unsigned int, void*);
typedef int (*chardev_open_t) (open_file_descriptor*);
typedef int (*chardev_close_t) (open_file_descriptor*);

/**
 * @brief Structure contenant les fonction qui permettent d'utiliser un char device.
 */
typedef struct {
	chardev_read_t read; /**< Fonction de lecture. */
	chardev_write_t write; /**< Fonction d'écriture. */
	chardev_ioctl_t ioctl; /**< Fonction de contrôle du périphérique. */
	chardev_open_t open; /**< Fonction appelée à l'ouverture du device. */
	chardev_close_t close; /**< Fonction appelée à la fermeture du device. */
	void * custom_data; /**< Données complémentaires qui peuvent être nécessaires pour utiliser le device. */
} chardev_interfaces;


typedef ssize_t (*blkdev_read_t)(open_file_descriptor *,void*, size_t, uint32_t);
typedef ssize_t (*blkdev_write_t)(open_file_descriptor *, const void*, size_t, uint32_t);
typedef int (*blkdev_ioctl_t) (open_file_descriptor*, unsigned int, void*);
typedef int (*blkdev_open_t) (open_file_descriptor*);
typedef int (*blkdev_close_t) (open_file_descriptor*);

/**
 * @brief Structure contenant les fonctions qui permettent d'utiliser un block device.
 */
typedef struct {
	blkdev_read_t read; /**< Fonction de lecture. */
	blkdev_write_t write; /**< Fonction d'écriture. */
	blkdev_ioctl_t ioctl; /**< Fonction de contrôle du périphérique. */
	blkdev_open_t open; /**< Fonction appelée à l'ouverture du device. */
	blkdev_close_t close; /**< Fonction appelée à la fermeture du device. */
	void * custom_data; /**< Données complémentaires qui peuvent être nécessaires pour utiliser le device. */
} blkdev_interfaces;

/**
 * @brief Initialisation du dev FS.
 */
void devfs_init();

/**
 * @brief Enregistre un char device dans le dev FS.
 */
int register_chardev(const char* name, chardev_interfaces* di);

/**
 * @brief Enregistre un block device dans le dev FS.
 */
int register_blkdev(const char* name, blkdev_interfaces* di);


#endif /* _DEVFS_H */
