/**
 * @file heap.h
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

#ifndef _HEAP_H_
#define _HEAP_H_

#include <types.h>

#define HEAP_MAX_SIZE 256 /**< Taille maximale du tas. */

/*
 * Structure de tas générique :
 * Ce tas est compatible avec n'importe quel type de données pour peu qu'on lui
 * donne une fonction de comparaison entre les elements.
 * Ce tas manipule uniquement des pointeurs vers les elements qu'il contient
 * Le tas n'est pas responsable de l'allocation et de la liberation de la mémoire
 * Il s'agit uniquement d'une structure de tri
 */

// maxheap : la fonction doit retourner >0 si a>b, 0 si a=b, <0 si a<b
// minheap : la fonction doit retourner <0 si a>b, 0 si a=b, >0 si a<b

/**
 *  Une fonction de comparaison entre 2 elements
 */
typedef int (*cmp_func_type) (void*, void*);

typedef int (*id_func_type) (int, void*);

/**
 * Structure de tas.
 */
typedef struct {
	cmp_func_type comparator; /**< Fonction de comparaison entre deux éléments. */
	void* heap; /**< Tableau qui contient le tas. */
	int nb_elements; /**< Nombre d'éléments dans le tas. */
	size_t elements_size; /**< Taille de chaque élément. */
	int max_elements; /**< Nombre maximum d'éléments. */
} heap_t;

/**
 *  Initialise le tas à vide et regle la fonction de comparaison
 */
void initHeap(heap_t* h, cmp_func_type cmp, void* heap, size_t elements_size,int max_elements);

/**
 *  Recupere le premier élément du tas.
 *
 *	@param h pointeur vers le tas.
 *
 *	@return pointeur vers le premier élément du tas, NULL si aucun.
 *
 */
void* getTop(heap_t *h);

/**
 *	Ajoute un element au tas.
 *
 *	@param h pointeur vers le tas.
 *	@param element pointeur vers l'élément à ajouter au tas.
 *
 *	@return 0 en cas de succes
 */
int addElement(heap_t* h, void* element);

/**
 *  Retire le sommet.
 *  @param h pointeur vers le tas.
 *
 *  @return 0 en cas de succes.
 */
int removetop(heap_t* h);

int delElement(heap_t* heap, int id, id_func_type func);

#endif
