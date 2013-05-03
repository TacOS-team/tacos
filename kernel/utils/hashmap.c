/**
 * @file hashmap.c
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

#include <kmalloc.h>
#include <hashmap.h>

hashmap_t* hashmap_create(int size, int (*equal)(struct hashmap_key_t*, struct hashmap_key_t*), int (*hash)(struct hashmap_key_t*)) {
	hashmap_t *this = kmalloc(sizeof(hashmap_t));
	this->size = size;
	this->table = kmalloc(sizeof(struct hashmap_cell_t*) * size);
	int i;
	for (i = 0; i < size; i++) {
		this->table[i] = NULL;	
	}
	this->equal = equal;
	this->hash = hash;
	return this;
}

void hashmap_set(hashmap_t* this, struct hashmap_key_t* key, struct hashmap_value_t* value) {
	int h = this->hash(key);
	struct hashmap_cell_t *p = this->table[h];
	while (p) {
		if (this->equal(p->key, key)) {
			p->value = value;
			return;
		}
		p = p->next;
	}
	struct hashmap_cell_t *e = kmalloc(sizeof(struct hashmap_cell_t));
	e->next = this->table[h];
	e->key = key;
	e->value = value;
	this->table[h] = e;
}

struct hashmap_value_t* hashmap_remove(hashmap_t* this, struct hashmap_key_t *key) {
	int h = this->hash(key);
	struct hashmap_cell_t *aux = this->table[h];
	struct hashmap_cell_t *prev = NULL;
	while (aux) {
		if (this->equal(aux->key, key)) {
			if (prev) {
				prev->next = aux->next;
			} else {
				this->table[h] = aux->next;
			}
			struct hashmap_value_t *value = aux->value;
			kfree(aux);
			return value;
		}
		prev = aux;
		aux = aux->next;
	}

	return NULL;
}

struct hashmap_value_t* hashmap_get(hashmap_t* this, struct hashmap_key_t *key) {
	int h = this->hash(key);
	struct hashmap_cell_t *aux = this->table[h];
	while (aux) {
		if (this->equal(aux->key, key)) {
			return aux->value;
		}
		aux = aux->next;
	}

	return NULL;
}

