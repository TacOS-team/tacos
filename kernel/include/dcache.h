/**
 * @file dcache.h
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
 * @brief Dentry cache.
 */

#ifndef _DCACHE_H
#define _DCACHE_H

struct _fs_instance_t;
struct _dentry_t;

/**
 * Initialisation du cache.
 */
void dcache_init();

/**
 * Cherche une valeur dans le cache.
 *
 * @param instance Instance de FS.
 * @param dentry Dentry parent.
 * @param name Nom de l'entrée.
 *
 * @return Dentry recherchée ou NULL si non en cache.
 */
struct _dentry_t *dcache_get(struct _fs_instance_t *instance, struct _dentry_t* dentry, const char * name);

/**
 * Insert une valeur dans le cache.
 *
 * @param instance Instance de FS.
 * @param pdentry Dentry parent.
 * @param name Nom de l'entrée.
 * @param dentry Dentry à mettre en cache.
 *
 */
void dcache_set(struct _fs_instance_t *instance, struct _dentry_t* pdentry, const char * name, struct _dentry_t *dentry);

/**
 * Supprime une valeur du cache.
 *
 * @param instance Instance de FS.
 * @param dentry Dentry parent.
 * @param name Nom de l'entrée.
 */
void dcache_remove(struct _fs_instance_t *instance, struct _dentry_t* dentry, const char * name);

#endif
