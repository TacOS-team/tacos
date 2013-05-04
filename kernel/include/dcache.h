/**
 * @file dcache.h
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
 * @brief dentry cache
 */

#ifndef _DCACHE_H
#define _DCACHE_H

struct _fs_instance_t;
struct _dentry_t;

void dcache_init();
struct _dentry_t *dcache_get(struct _fs_instance_t *instance, struct _dentry_t* dentry, const char * name);
void dcache_set(struct _fs_instance_t *instance, struct _dentry_t* pdentry, const char * name, struct _dentry_t *dentry);
void dcache_remove(struct _fs_instance_t *instance, struct _dentry_t* dentry, const char * name);

#endif
