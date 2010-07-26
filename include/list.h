#ifndef _LIST_H_
#define _LIST_H_

#include <types.h>

typedef int (*cmp_func_type) (void*, void*);
typedef int (*id_func_type) (int, void*);

typedef struct {
	
	cmp_func_type comparator; /* Fonction de comparaison des elements */
	
	int head; 				/* Indice de la tête de liste */
	void* elements_array; 	/* Tableau contenant les éléments */
	int* link_array; 		/* Tableau contenant les liens entre les éléments,sers aussi à savoir si un emplacement est vide */
	
	int nb_elements; 		/* Nombre d'éléments présents dans la liste */
	size_t elements_size; 	/* Taille du type d'élement à contenir */
	int max_elements; 		/* Nombre maximum d'élements dans la liste */
} list_t;

void initList(list_t* l, cmp_func_type cmp, size_t elements_size,int max_elements);
void* listGetTop(list_t l);
int listAddElement(list_t* l, void* element);
int listRemoveTop(list_t* l);
int listDelElement(list_t* list, int id, id_func_type func);


void print_list(list_t ma_liste);
#endif
