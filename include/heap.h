#ifndef _HEAP_H_
#define _HEAP_H_

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

typedef struct {
	cmp_func_type comparator;
	void* heap[HEAP_MAX_SIZE];
	int nb_elements;
} heap_t;

// Initialise le tas à vide et regle la fonction de comparaison
void initHeap(heap_t* h, cmp_func_type cmp);

// Recupere le pointeur du sommet, NULL sinon
void* getTop(heap_t h);

// Ajoute un element, retourne 0 en cas de succes
int addElement(heap_t* h, void* element);

// Retire le sommet, retourne 0 en cas de succes
int removetop(heap_t* h);

#endif
