#ifndef _HEAP_H_
#define _HEAP_H_

#define HEAP_MAX_SIZE 256

typedef int (*cmp_func_type) (void*, void*);

typedef struct {
	cmp_func_type comparator;
	void* heap[HEAP_MAX_SIZE];
	int nb_elements;
} heap_t;

void initHeap(heap_t* h, cmp_func_type cmp);
void* getTop(heap_t h);
int addElement(heap_t* h, void* element);
int removetop(heap_t* h);

#endif
