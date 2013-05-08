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

/**
 * Pour chaque valeur de hash, on retrouve une liste chainée dont les
 * éléments sont du type hashmap_cell_t.
 */
struct hashmap_cell_t {
	struct hashmap_key_t *key;
	struct hashmap_value_t *value;
	struct hashmap_cell_t *next;
};

/**
 * Structure représentant une hashmap.
 */
typedef struct __hashmap_t {
	int size;
	struct hashmap_cell_t** table;
	int (*equal)(struct hashmap_key_t*, struct hashmap_key_t*);
	int (*hash)(struct hashmap_key_t*);
} hashmap_t;

/**
 * Crée et initialise une nouvelle hashmap.
 *
 * @param size La taille de la table, correspond à la valeur maximale - 1 que peut
 *  retourner la fonction de hash.
 * @param equal Fonction qui retourne 1 si 2 clefs sont identiques.
 * @param hash Fonction de hash.
 *
 * @return Pointeur vers la hashmap.
 */
hashmap_t* hashmap_create(int size, int (*equal)(struct hashmap_key_t*, struct hashmap_key_t*), int (*hash)(struct hashmap_key_t*));

/**
 * Ajoute ou modifie une valeur dans la hashmap.
 *
 * @param this Pointeur vers la hashmap à utiliser.
 * @param key La clef.
 * @param value La valeur.
 */
void hashmap_set(hashmap_t* this, struct hashmap_key_t* key, struct hashmap_value_t* value);

/**
 * Supprime un élément de la hashmap. Attention, seule la cellule est freed.
 *
 * @param this Pointeur vers la hashmap à utiliser.
 * @param key La clef de l'élément à supprimer.
 *
 * @return Pointeur vers la valeur enlevée de la hashmap, NULL si non trouvée.
 */
struct hashmap_value_t* hashmap_remove(hashmap_t* this, struct hashmap_key_t *key);

/**
 * Lit un élément de la hashmap.
 *
 * @param this Pointeur vers la hashmap à utiliser.
 * @param key La clef de l'élément à lire.
 *
 * @return Pointeur vers la valeur, NULL si non trouvée.
 */
struct hashmap_value_t* hashmap_get(hashmap_t* this, struct hashmap_key_t *key);

#endif
