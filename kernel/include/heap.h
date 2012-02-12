/**
 * @file heap.h
 *
 * @author TacOS developers 
 *
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

#define HEAP_MAX_SIZE 256

/*
 * Structure de tas générique :
 * Ce tas est compatible avec n'importe quel type de données pour peu qu'on lui
 * donne une fonction de comparaison entre les elements.
 * Ce tas manipule uniquement des pointeurs vers les elements qu'il contient
 * Le tas n'est pas responsable de l'allocation et de la liberation de la mémoire
 * Il s'agit uniquement d'une structure de tri
 */

// Une fonction de comparaison entre 2 elements
// maxheap : la fonction doit retourner >0 si a>b, 0 si a=b, <0 si a<b
// minheap : la fonction doit retourner <0 si a>b, 0 si a=b, >0 si a<b
typedef int (*cmp_func_type) (void*, void*);
typedef int (*id_func_type) (int, void*);

typedef struct {
	cmp_func_type comparator;
	void* heap;
	int nb_elements;
	size_t elements_size;
	int max_elements;
} heap_t;

// Initialise le tas à vide et regle la fonction de comparaison
void initHeap(heap_t* h, cmp_func_type cmp, void* heap, size_t elements_size,int max_elements);

// Recupere le pointeur du sommet, NULL sinon
void* getTop(heap_t h);

// Ajoute un element, retourne 0 en cas de succes
int addElement(heap_t* h, void* element);

// Retire le sommet, retourne 0 en cas de succes
int removetop(heap_t* h);

int delElement(heap_t* heap, int id, id_func_type func);

#endif
