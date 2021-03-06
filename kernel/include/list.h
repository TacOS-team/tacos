/**
 * @file list.h
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
 * Structure de liste chainée générique.
 */

#ifndef _LIST_H_
#define _LIST_H_

#include <types.h>

/**
 * Définition du type pour une fonction de comparaison entre 2 éléments.
 */
typedef int (*cmp_func_type) (void*, void*);
typedef int (*id_func_type) (int, void*);

/**
 * @brief Liste générique.
 */
typedef struct {
	cmp_func_type comparator; /**< Fonction de comparaison des elements */

	int head; 				/**< Indice de la tête de liste */
	void* elements_array; 	/**< Tableau contenant les éléments */
	int* link_array; 		/**< Tableau contenant les liens entre les éléments,sers aussi à savoir si un emplacement est vide */
	
	int nb_elements; 		/**< Nombre d'éléments présents dans la liste */
	size_t elements_size; 	/**< Taille du type d'élement à contenir */
	int max_elements; 		/**< Nombre maximum d'élements dans la liste */
} list_t;

/**
 * Initialisation d'un liste.
 *
 * @param l La liste manipulée.
 * @param cmp Fonction de comparaison.
 * @param elements_size Taille d'un élément de la liste.
 * @param max_elements Nombre max d'éléments dans la liste.
 */
void list_init(list_t* l, cmp_func_type cmp, size_t elements_size, int max_elements);

/**
 * Récupère le premier élément de la liste.
 *
 * @param l La liste manipulée.
 */
void* list_get_top(list_t l);

/**
 * Ajoute un élément à la fin de la liste.
 *
 * @param l La liste manipulée.
 * @param element Element à ajouter.
 *
 * @return 0 en cas de succès.
 */
int list_add_element(list_t* l, void* element);

/**
 * Retire le premier élément de la liste.
 *
 * @param l La liste manipulée.
 *
 * @return 0 en cas de succès.
 */
int list_remove_top(list_t* l);

/**
 * Suppression d'un élément de la liste.
 *
 * @param list La liste manipulée.
 *
 * @return 0 en cas de succès.
 */
int list_del_element(list_t* list, int id, id_func_type func);

/**
 * Affichage d'une liste (pour debug).
 *
 * @param ma_liste Liste à afficher.
 */
void print_list(list_t ma_liste);

#endif //_LIST_H_
