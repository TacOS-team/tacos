/**
 * @file fs/ext2.h
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

#endif
