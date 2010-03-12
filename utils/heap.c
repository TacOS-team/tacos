#include "heap.h"
#include "types.h"

// initialise le tas
void initHeap(heap_t* h, cmp_func_type cmp)
{
	h->nb_elements = 0;
	h->comparator = cmp;
}

// retourne le sommet
void* getTop(heap_t h)
{
	void* top = NULL;
	
	if(h.nb_elements>0)
		top = h.heap[0];
		
	return top;
}

// alterne deux elements d'indice a,b de tab
void swap(int a, int b, void** tab)
{
	void* tmp;
	tmp = tab[a];
	tab[a] = tab[b];
	tab[b] = tmp;
}

// ajoute une element
int addElement(heap_t* h, void* element)
{
	// On verifie que le tas n'est pas plein
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
			// tant que l'element est plus grand que son pere on le remonte
			swap(index, (index-1)/2, h->heap);

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
	// si le tas n'est pas deja vide
	if(h->nb_elements > 0)
	{
		h->nb_elements--;
		
		// si on n'a pas totalement vidé le tas
		if(h->nb_elements > 0)
		{
			int finished = 0;
			int index = h->nb_elements;
			void* tmp;
			
			// On place le dernier element au sommet
			h->heap[0] = h->heap[index];
			
			// On le redescend
			index = 0;
			while(!finished)
			{
				int son1 = (2*index)+1;
				int son2 = (2*index)+2;
				
				// si on a un fils
				if(son1 < h->nb_elements)
				{
					// si on a un second fils
					if(son2 < h->nb_elements)
					{
						// On a deux fils
						if(h->comparator(h->heap[son1],h->heap[index]) > 0)
						{
							if(h->comparator(h->heap[son2],h->heap[son1]) > 0)
							{
								// moi < fils1 < fils2
								// on alterne moi et fils2
								swap(index, son2, h->heap);
								index = son2;
							}
							else
							{
								// moi < fils2 < fils1
								// on alterne moi et fils1
								swap(index, son1, h->heap);
								index = son1;
							}
						}
						else if(h->comparator(h->heap[son2],h->heap[index]) > 0)
						{
							// fils1 < moi < fils2
							// on alterne moi et fils2
							swap(index, son2, h->heap);
							index = son2;
						}
						else
						{
							// fils1 & fils2 < moi
							// on a fini de descendre
							finished = 1;
						}
					}
					else
					{
						// On a un seul fils
						if(h->comparator(h->heap[son1],h->heap[index]) > 0)
						{
							// fils > moi
							// On alterne
							swap(index, son1, h->heap);
							index = son1;
						}
						else
						{
							// moi > fils
							// on a fini de descendre
							finished = 1;
						}
					}
				}
				else
				{
					// Plus de fils
					// on a fini de descendre
					finished = 1;
				}
			}
		}
		return 0;
	}
	
	return 1;
}
