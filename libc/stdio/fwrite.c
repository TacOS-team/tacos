#include <stdio.h>

/**
 *	Cette fonction écrit jusqu'à nmemb elements dont la taille est 
 *	définie par size vers le stream pointé par le pointeur stream.
 *	Pour chaque elements, fwrite fait size appels à fputc en castant 
 *	simplement l'element en un tableau de uchar.
 */
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t i,j;

	for(i = 0; i < nmemb; i++) {
		unsigned char * element = &(ptr[i*size]);
		for(j = 0; j < size; j++) {
			fputc(element[j], stream);
		}
	}
	return -1;
}
