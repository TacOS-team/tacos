/**
 * @file dcache.c
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
 * @brief dentry cache
 */

#include <hashmap.h>
#include <klibc/string.h>
#include <vfs.h>
#include <kmalloc.h>

static hashmap_t *map;

struct key_t {
	fs_instance_t* instance; /**< FS Instance. */
	dentry_t* dentry; /**< Parent directory entry. */
	const char* name; /**< Dir name. */
};

static int dentry_equal(struct hashmap_key_t *a, struct hashmap_key_t *b) {
	struct key_t *ka = (struct key_t*) a;
	struct key_t *kb = (struct key_t*) b;
	return (a && b && ka->instance == kb->instance
									&& ka->dentry->d_inode->i_ino == kb->dentry->d_inode->i_ino
									&& strcmp(ka->name, kb->name) == 0);
}

static int dentry_hash(struct hashmap_key_t* key) {
	return ((struct key_t*)key)->dentry->d_inode->i_ino % 100;
}

void dcache_init() {
	map = hashmap_create(100, dentry_equal, dentry_hash);
}

void dcache_remove(struct _fs_instance_t *instance, struct _dentry_t* dentry, const char * name) {
	if (!instance->fs->unique_inode) return;
	struct key_t key;
	key.instance = instance;
	key.dentry = dentry;
	key.name = name;
	hashmap_remove(map, (struct hashmap_key_t*)&key);
}

struct _dentry_t *dcache_get(struct _fs_instance_t *instance, struct _dentry_t* dentry, const char * name) {
	if (!instance->fs->unique_inode) return NULL;
	struct key_t key;
	key.instance = instance;
	key.dentry = dentry;
	key.name = name;

	// TODO Il faudrait repositionner l'élément au top du LRU mais pour cela il
	// faudrait stocker dans la hashmap l'index sinon ça implique un parcours 
	// ce qui casse un peu tout l'intérêt de la hashmap...
	return (dentry_t*)hashmap_get(map, (struct hashmap_key_t*)&key);
}

void dcache_set(struct _fs_instance_t *instance, struct _dentry_t* pdentry, const char * name, struct _dentry_t *dentry) {
	if (!instance->fs->unique_inode) return;
	struct key_t *key = kmalloc(sizeof(struct key_t));
	key->instance = instance;
	key->dentry = pdentry;
	key->name = name;

	hashmap_set(map, (struct hashmap_key_t*)key, (struct hashmap_value_t*)dentry);

}
