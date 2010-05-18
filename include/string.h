#ifndef _STRING_H_
#define _STRING_H_

#include <types.h>

/** 
 * @brief Copie une zone mémoire.
 *
 * La fonction memcpy() copie une zone mémoire de size octets depuis src vers
 * la zone mémoire dest. Les deux zones ne doivent pas se chevaucher sinon il 
 * faut utiliser memmove.
 * 
 *	@see memmove
 *
 * @param dest adresse mémoire où il faut copier
 * @param src adresse mémoire source
 * @param size taille de la zone à copier
 * 
 * @return pointeur sur dest.
 */
void* memcpy(void* dest, const void* src, size_t size);

size_t strlen(const char* s);

int strcmp(const char *s1, const char *s2);

int strncmp(const char *s1, const char *s2, size_t n);

void *memset(void *s, int c, size_t n);

int memcmp(const void *s1, const void *s2, size_t n);

char *strcpy(char * s1, const char * s2);

char *strncpy(char * s1, const char * s2, size_t n);

char *strcat(char * s1, const char * s2);

char *strncat(char * s1, const char * s2, size_t n);

void *memmove(void *dest, const void *src, size_t n);

char *strstr(const char *haystack, const char *needle);

char *strcasestr(const char *haystack, const char *needle);

char *strdup (const char *s);

char *strchr(const char *s, int c);

/* NOT IMPLEMENTED YET

int strcoll(const char *s1, const char *s2);

size_t strxfrm(char * s1, const char * s2, size_t n);

void *memchr(const void *s, int c, size_t n);

size_t strcspn(const char *s1, const char *s2);

char *strpbrk(const char *s1, const char *s2);

char *strrchr(const char *s, int c);

size_t strspn(const char *s1, const char *s2);

char *strtok(char * s1, const char * s2);

char *strerror(int errnum);

*/

#endif
