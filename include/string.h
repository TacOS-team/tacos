#ifndef _STRING_H_
#define _STRING_H_

#include <types.h>

void* memcpy(void* dest, const void* src, size_t size);

size_t strlen(const char* s);

int strcmp(const char *s1, const char *s2);

int strncmp(const char *s1, const char *s2, size_t n);

void *memset(void *s, int c, size_t n);

int memcmp(const void *s1, const void *s2, size_t n);

char *strcpy(char * s1, const char * s2);

char *strncpy(char * s1, const char * s2, size_t n);

char *strcat(char * s1, const char * s2);

/* NOT IMPLEMENTED YET

char *strncat(char * s1, const char * s2, size_t n);

void *memmove(void *s1, const void *s2, size_t n);

int strcoll(const char *s1, const char *s2);

size_t strxfrm(char * s1, const char * s2, size_t n);

void *memchr(const void *s, int c, size_t n);

char *strchr(const char *s, int c);

size_t strcspn(const char *s1, const char *s2);

char *strpbrk(const char *s1, const char *s2);

char *strrchr(const char *s, int c);

size_t strspn(const char *s1, const char *s2);

char *strstr(const char *s1, const char *s2);

char *strtok(char * s1, const char * s2);

char *strerror(int errnum);

*/

#endif
