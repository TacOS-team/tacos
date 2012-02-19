/**
 * @file fs/ext2.h
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
 * @brief Driver Ext2.
 */

#ifndef _EXT2_H_
#define _EXT2_H_

#include <vfs.h>

/**
 * @brief Initialisation du driver ext2.
 */
void ext2_init();

/**
 * @brief Ajoute un point de montage.
 *
 * @param ofd OFD du device que l'on veut monter.
 *
 * @return Une instance de FS.
 */
fs_instance_t* mount_EXT2(open_file_descriptor* ofd);

/**
 * @brief Unmount d'un point de montage.
 *
 * @param instance L'instance de fs que l'on veut démonter.
 */
void umount_EXT2(fs_instance_t *instance);

/**
 * @brief Ouverture de fichier.
 *
 * @param path Chemin (relatif au point de montage) du fichier à ouvrir.
 * @param flags Flags pour l'ouverture.
 *
 * @return un open file descriptor.
 */
open_file_descriptor * ext2_open(fs_instance_t *instance, const char * path, uint32_t flags);

/**
 * @brief Lecture d'un fichier.
 *
 * @param ofd Descripteur du fichier ouvert.
 * @param buf Buffer où stocker les octets lus.
 * @param size Nombre d'octets à lire.
 *
 * @return nombre d'octets réellement lus.
 */
size_t ext2_read(open_file_descriptor * ofd, void * buf, size_t size);

/**
 * @brief Fermeture d'un fichier ouvert.
 *
 * @param ofd Descripteur du fichier ouvert.
 *
 * @return 0 en cas de succès, -1 sinon.
 */
int ext2_close(open_file_descriptor *ofd);

/**
 * @brief Lecture du contenu d'un dossier.
 *
 * @param ofd Descripteur de fichier qui correspond au dossier.
 * @param entries Buffer qui va recevoir les entrées de dossier.
 * @param size Nombre d'octets dans le buffer au maximum.
 *
 * @return Nombre d'octets écrits dans entries.
 */
int ext2_readdir(open_file_descriptor * ofd, char * entries, size_t size);

/**
 * @brief Récupère l'état d'un fichier ou dossier.
 *
 * @param instance Instance de fs.
 * @param path Chemin du fichier.
 * @param stbuf Endroit où enregistrer les infos.
 *
 * @return 0 en cas de succès.
 */
int ext2_stat(fs_instance_t *instance, const char *path, struct stat *stbuf);

/**
 * @brief Création d'un noeud (fichier, dossier, fichier spécial...)
 *
 * @param instance Instance de fs.
 * @param path Chemin du noeud.
 * @param dev En cas de fichier spécial (device)
 *
 * @return 0 en cas de succès.
 */
int ext2_mknod(fs_instance_t *instance, const char * path, mode_t mode, dev_t dev);

/**
 * @brief Création d'un dossier.
 *
 * @param instance Instance de fs.
 * @param path Chemin du dossier à créer.
 * @param mode Droits sur le dossier.
 *
 * @return 0 en cas de succès.
 */
int ext2_mkdir(fs_instance_t *instance, const char * path, mode_t mode);

/**
 * @brief Suppression d'un noeud.
 *
 * @param instance Instance de fs.
 * @param path Chemin du noeud.
 *
 * @return 0 en cas de succès.
 */
int ext2_unlink(fs_instance_t *instance, const char * path);

/**
 * @brief Change la taille d'un fichier.
 *
 * @param instance Instance de fs.
 * @param path Chemin du fichier.
 * @param off Nouvelle taille.
 *
 * @return 0 en cas de succès.
 */
int ext2_truncate(fs_instance_t *instance, const char * path, off_t off);

/**
 * @brief Suppression d'un dossier vide.
 *
 * @param instance Instance de fs.
 * @param path Chemin du dossier à supprimer.
 *
 * @return 0 en cas de succès.
 */
int ext2_rmdir(fs_instance_t *instance, const char * path);

#endif
