#include <stdlib.h>
#include <stdio.h>
#include "customs.h"

void *cmalloc(size_t size)
{
	void *space = NULL;

	if (size == 0) {
		perror("Malloc null size");
		exit(BAD_ALLOC);
	}
	space = malloc(size);
	if (space == NULL) {
		perror("Bad memory allocation");
		exit(BAD_ALLOC);
	}

	return space;
}

void cfree(void *p)
{
	if (p == NULL) {
		perror("Attempt to free NULL pointer");
		exit(FREE_NULL);
	}
	free(p);
}
