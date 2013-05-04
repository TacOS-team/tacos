/**
 * @file hashmap.h
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
 * Hashmap générique. Des tests un peu plus poussés sont à faire.
 */

#ifndef _HASHMAP_H
#define _HASHMAP_H

// Types incomplets, juste pour y voir plus clair.
struct hashmap_key_t;
struct hashmap_value_t;

struct hashmap_cell_t {
	struct hashmap_key_t *key;
	struct hashmap_value_t *value;
	struct hashmap_cell_t *next;
};

typedef struct __hashmap_t {
	int size;
	struct hashmap_cell_t** table;
	int (*equal)(struct hashmap_key_t*, struct hashmap_key_t*);
	int (*hash)(struct hashmap_key_t*);
} hashmap_t;

hashmap_t* hashmap_create(int size, int (*equal)(struct hashmap_key_t*, struct hashmap_key_t*), int (*hash)(struct hashmap_key_t*));
void hashmap_set(hashmap_t* this, struct hashmap_key_t* key, struct hashmap_value_t* value);
struct hashmap_value_t* hashmap_remove(hashmap_t* this, struct hashmap_key_t *key);
struct hashmap_value_t* hashmap_get(hashmap_t* this, struct hashmap_key_t *key);

#endif
