/**
 * @file fat_outils.h
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
 * @brief Fonctions d'aide pour le driver FAT.
 */

#ifndef _FAT_OUTILS_H
#define _FAT_OUTILS_H

#include "fat_internal.h"

/**
 * Renvoie l'adresse utilisée pour indiquer que c'est le dernier cluster.
 *
 * @param instance de FS.
 *
 * return l'adresse.
 */
int fat_last_cluster(fat_fs_instance_t *instance);

/**
 * Indique si le cluster est libre.
 *
 * @param cluster Adresse du cluster.
 *
 * @return vrai ou faux.
 */
int fat_is_free_cluster(int cluster);

/**
 * Indique si c'est le dernier cluster.
 *
 * @param instance de FS.
 * @param cluster Adresse du cluster.
 *
 * @return vrai ou faux.
 */
int fat_is_last_cluster(fat_fs_instance_t *instance, int cluster);

/**
 * Indique si c'est le dernier cluster.
 *
 * @param instance de FS.
 * @param cluster Adresse du cluster.
 *
 * @return vrai ou faux.
 */
int fat_is_used_cluster(fat_fs_instance_t *instance, int cluster);

/**
 * Encode un nom long.
 *
 * @param name Name to encode.
 * @param long_file_name Name encoded.
 * @param n_entries Nombre d'entrées nécessaires.
 */
void fat_encode_long_file_name(char * name, lfn_entry_t * long_file_name, int n_entries);

/**
 * Décode un nom long.
 *
 * @param name Buffer de sortie contenant le nom long.
 * @param long_file_name Tableau de lfn_entry_t.
 *
 * @return name en cas de succès.
 */
char * fat_decode_long_file_name(char * name, lfn_entry_t * long_file_name);
time_t convert_datetime_fat_to_time_t(fat_date_t *date, fat_time_t *time);
void convert_time_t_to_datetime_fat(time_t time, fat_time_t *timefat, fat_date_t *datefat);
void fat_dir_entry_to_directory_entry(char *filename, fat_dir_entry_t *dir, directory_entry_t *entry);

/**
 * Converti un nom de fichier long en nom court.
 *
 * @param filename nom long.
 * 
 * @return nom court.
 */
char * fat_lfn_to_sfn(char * filename);

/**
 * Décodage d'un nom court.
 */
void fat_decode_short_file_name(char *filename, fat_dir_entry_t *fdir);

/**
 * Décode une entrée de type long file name.
 *
 * @param fdir Tableau contenant des entrées de type lfn. 
 *
 * @return les informations du dossier sous la forme d'un directory_entry_t.
 */
directory_entry_t * fat_decode_lfn_entry(lfn_entry_t* fdir);

/**
 * Décode une entrée de type short file name.
 *
 * @param fdir Pointeur sur une entrée de dossier.
 *
 * @return les informations du dossier sous la forme d'un directory_entry_t.
 */
directory_entry_t * fat_decode_sfn_entry(fat_dir_entry_t *fdir);

/**
 * Découpe un chemin en deux parties (dossier séparé du filename).
 *
 * @param path Chemin à découper.
 * @param dir Première partie qui va jusqu'au dernier slash du path.
 * @param filename Dernière partie (après le dernier slash).
 *
 */
void fat_split_dir_filename(const char * path, char * dir, char * filename);

#endif
