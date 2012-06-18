/**
 * @file fs/fat.h
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

#ifndef _KFAT_H_
#define _KFAT_H_

#include <types.h>
#include <fd_types.h>
#include <kstat.h>
#include <vfs.h>

/**
 * Initialisation du FS FAT.
 */
void fat_init();

/**
 * Monte un FS.
 *
 * @param ofd Descripteur de fichier pointant sur le device à monter.
 *
 * @return Instance de FS.
 */
fs_instance_t* mount_FAT(open_file_descriptor* ofd);

/**
 * Démonte un FS.
 * 
 * @param instance Instance de FS.
 */
void umount_FAT(fs_instance_t *instance);

/**
 * Lecture d'un dossier.
 *
 * @param ofd Descripteur de fichier qui correspond au dossier.
 * @param entries Buffer qui va recevoir les entrées de dossier.
 * @param size Nombre d'octets dans le buffer au maximum.
 *
 * @return Nombre d'octets écrits dans entries.
 */
int fat_readdir(open_file_descriptor * ofd, char * entries, size_t size);

/**
 * Ouverture d'un fichier.
 *
 * @param instance Instance du fs.
 * @param path Chemin (relatif au point de montage) du fichier à ouvrir.
 * @param flags Flags pour l'ouverture.
 *
 * @return un open file descriptor.

 */
open_file_descriptor * fat_open_file(fs_instance_t *instance, const char * path, uint32_t flags);

/**
 * Création d'un dossier.
 *
 * @param instance Instance de fs.
 * @param path Chemin du dossier à créer.
 * @param mode Droits sur le dossier.
 *
 * @return 0 en cas de succès.
 */
int fat_mkdir(fs_instance_t *instance, const char * path, mode_t mode);

/**
 * Informations sur un fichier.
 */
int fat_stat(fs_instance_t *instance, const char *path, struct stat *stbuf);

/**
 * Suppression d'un fichier ou dossier (vide).
 *
 * @param instance Instance de fs.
 * @param path Chemin du noeud.
 *
 * @return 0 en cas de succès.
 */
int fat_unlink(fs_instance_t *instance, const char * path);

/**
 * Lecture d'un fichier.
 *
 * @param ofd Descripteur du fichier ouvert.
 * @param buf Buffer où stocker les octets lus.
 * @param size Nombre d'octets à lire.
 *
 * @return nombre d'octets réellement lus.
 */
size_t fat_read_file (open_file_descriptor * ofd, void * buf, size_t count);

/**
 * Écriture de fichier.
 */
size_t fat_write_file (open_file_descriptor * ofd, const void * buf, size_t nb_octet);

/**
 * Déplacement dans un fichier.
 */
int fat_seek_file (open_file_descriptor * ofd, long offset, int whence);

/**
 * Fermeture d'un fichier ouvert.
 *
 * @param ofd Descripteur de fichier ouvert.
 *
 * @return 0 en cas de succès.
 */
int fat_close(open_file_descriptor *ofd);

#endif
