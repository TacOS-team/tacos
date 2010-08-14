#ifndef _STDLIB_H_
#define _STDLIB_H_

/**
 * @file stdlib.h
 */

#include <types.h>

#define RAND_MAX 2147483647

void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
int free(void *addr);
long int strtol(const char* nptr, char** endptr, int base);
unsigned long int strtoul(const char *nptr, char **endptr, int base);
int atoi(const char* __nptr);


int rand(void);
void srand(unsigned int seed);

#endif

