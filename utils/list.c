/*
 * TESTS UNITAIRES OK
 */


#include <list.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define EMPTY_SLOT -1
#define END_SLOT -2



void initList(list_t* l, cmp_func_type cmp, size_t elements_size,int max_elements)
{
	int i;
	
	l->comparator = cmp;
	l->nb_elements = 0;
	l->max_elements = max_elements;
	l->elements_array = kmalloc(max_elements * elements_size);
	l->link_array = kmalloc(max_elements * sizeof(int) );
	
	for(i=0; i<max_elements; i++)
		l->link_array[i] = EMPTY_SLOT;
	l->head = EMPTY_SLOT;
		
	l->elements_size = elements_size;
}

void* getElement(list_t* l,int index)
{
	return l->elements_array + (index * l->elements_size);
}

void* listGetTop(list_t l)
{
	void* top = NULL;
	
	if(l.nb_elements>0)
		top = getElement(&l,l.head);
		
	return top;
}

int listAddElement(list_t* l, void* element)
{
	
	int ret = 1;
	int i=0, j=0;	

	if(l->nb_elements < l->max_elements)
	{	
		/* Insertion de l'élément dans le tableau de données */
		while(l->link_array[i] != EMPTY_SLOT)
			i++;
		memcpy(getElement(l,i), element, l->elements_size);
		
		/* Si la liste est vide */
		if( l->nb_elements == 0 )
		{
			l->link_array[i] = END_SLOT;
			l->head = i;
		}
		/* Si l'élément se place en tête */
		else if(l->comparator(element,getElement(l,l->head)) <= 0 )  { 
			l->link_array[i] = l->head;
			l->head = i;
		}
		else {
			j = l->head;
			/* On cherche la position du nouvel élément */
			
			while(l->link_array[j]!=END_SLOT && l->comparator(element, getElement(l,l->link_array[j])) > 0 )
			{
				j = l->link_array[j];
			}	
			
			/* Et on fait le lien */
			l->link_array[i] = l->link_array[j];
			l->link_array[j] = i;
			
		}
		l->nb_elements ++;	
		
		ret = 0;
	}
	return ret;	
}

int listRemoveTop(list_t* l)
{
	int ret = 1;
	int head = l->head;
	if(l->nb_elements > 0)
	{
		l->head = l->link_array[head];
		l->link_array[head] = EMPTY_SLOT;
		l->nb_elements--;
		ret = 0;
	}
	
	return ret;
}

void print_list(list_t ma_liste)
{
	int a;
	a = ma_liste.head;
	int *i;
	while(a != END_SLOT)
	{
		i = getElement(&ma_liste, a);
		printf("%d ", *i );
		a = ma_liste.link_array[a];
	}
}

int listDelElement(list_t* list, int id, id_func_type func)
{
	int el_index = 0;
	int i = 0;
	
	while(!func(id,getElement(list,el_index)))
		el_index++;
		
	while(list->link_array[i] != el_index)
		i++;
	
	list->link_array[i] = list->link_array[el_index];
	
	list->link_array[el_index] = EMPTY_SLOT;
	
	list->nb_elements--;
	
	return 0; /* TODO: bonne valeur de retour */
}

