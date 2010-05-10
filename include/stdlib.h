#ifndef _MALLOC_H_
#define _MALLOC_H_

#include <types.h>

void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
int free(void *addr);
long int strtol(const char* nptr, char** endptr, int base);
unsigned long int strtoul(const char *nptr, char **endptr, int base);
int atoi(const char* __nptr);

#endif

