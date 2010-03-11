#include "heap.h"
#include "types.h"

void initHeap(heap_t* h, cmp_func_type cmp)
{
	h->nb_elements = 0;
	h->comparator = cmp;
}

void* getTop(heap_t h)
{
	void* top = NULL;
	
	if(h.nb_elements>0)
		top = h.heap[0];
		
	return top;
}

int addElement(heap_t* h, void* element)
{
	if(h->nb_elements < HEAP_MAX_SIZE)
	{
		int index = h->nb_elements;
		void* tmp;
		
		// On insere l'element en fin de tableau
		h->nb_elements++;
		h->heap[index] = element;
		
		// On remonte l'element si necessaire
		while(h->comparator(h->heap[index],h->heap[(index-1)/2]) > 0)
		{
			tmp = h->heap[index];
			h->heap[index] = h->heap[(index-1)/2];
			h->heap[(index-1)/2] = tmp;
			index = (index-1)/2;
			
			if(index == 0)
				break;
		}
		
		return 0;
	}
	
	return 1;
}

int removetop(heap_t* h)
{
	if(h->nb_elements > 0)
	{
		h->nb_elements--;
		
		if(h->nb_elements > 0)
		{
			int index = h->nb_elements;
			void* tmp;
			
			// On place le dernier element au sommet
			h->heap[0] = h->heap[index];
			
			// On le redescend
			while(h->comparator(h->heap[(2*index)+1],h->heap[index]) > 0)
			{
				tmp = h->heap[index];
				h->heap[index] = h->heap[(2*index)+1];
				h->heap[(2*index)+1] = tmp;
				index = (2*index)+1;
				
				if((2*index)+1 >= h->nb_elements)
					break;
			}
		}
		return 0;
	}
	
	return 1;
}
