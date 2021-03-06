/**
 * @file fs/fat.h
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

#endif
