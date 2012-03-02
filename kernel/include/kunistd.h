/**
 * @file kunistd.h
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

#ifndef _KUNISTD_H
#define _KUNISTD_H

#include <types.h>
#include <ksyscall.h>
#include <kstat.h>

/**
 * Syscall pour écrire dans un fichier.
 *
 * @param fd Id du descripteur de fichier.
 * @param buf Données à écrire.
 * @param c Nombre d'octets à écrire.
 */
SYSCALL_HANDLER3(sys_write, uint32_t fd, const void *buf, size_t *c);

/**
 * Syscall pour lire dans un fichier.
 *
 * @param fd Id du descripteur de fichier.
 * @param buf Buffer pour stocker les données lues.
 * @param c Nombre d'octets à lire.
 */
SYSCALL_HANDLER3(sys_read, uint32_t fd, const void *buf, size_t *c);

/**
 * Syscall pour se déplacer dans un fichier.
 *
 * @param fd Id du descripteur de fichier.
 */
SYSCALL_HANDLER3(sys_seek, uint32_t fd, long *offset, int whence);

/**
 * Syscall pour controler un fichier.
 *
 * @param fd Id du descripteur de fichier.
 */
SYSCALL_HANDLER3(sys_ioctl, uint32_t fd, unsigned int request, void *data);

/**
 * Syscall pour obtenir des informations sur un fichier.
 */
SYSCALL_HANDLER3(sys_stat, const char *path, struct stat *buf, int *ret);

/**
 * Syscall pour supprimer un fichier.
 */
SYSCALL_HANDLER2(sys_unlink, const char *path, int *ret);

/**
 * Syscall pour dupliquer un open file descriptor.
 */
SYSCALL_HANDLER2(sys_dup, int oldfd, int *ret);

/**
 * Syscall pour créer un nouveau noeud.
 */
SYSCALL_HANDLER3(sys_mknod, const char *path, mode_t mode, dev_t *dev);

#endif
