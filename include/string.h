#ifndef _STRING_H_
#define _STRING_H_

/**
 * @file string.h
 *
 */

#include <types.h>

/** 
 * @brief Copie une zone mémoire.
 *
 * La fonction memcpy() copie une zone mémoire de size octets depuis src vers
 * la zone mémoire dest. Les deux zones ne doivent pas se chevaucher sinon il 
 * faut utiliser memmove.
 * 
 * @see memmove
 *
 * @param dest adresse mémoire où il faut copier
 * @param src adresse mémoire source
 * @param size taille de la zone à copier
 * 
 * @return un pointeur sur dest.
 */
void* memcpy(void* dest, const void* src, size_t size);

/** 
* @brief Calcule la longueur d'une chaîne de caractères.
*
* La fonction strlen() calcule la longueur de la chaîne de caractères s sans
* compter le caractère nul « \0 »
* 
* @param s la chaîne dont on veut calculer la longueur
* 
* @return le nombre de caractères dans la chaîne s.
*/
size_t strlen(const char* s);

/** 
* @brief Compare deux chaînes.
* 
* La fonction strlen() compare deux chaînes de caractère et retourne en 
* résultat un nombre négatif, positif ou nul si s1 est respectivement
* inférieur, supérieur ou identique à s2.
* 
* @param s1 première chaîne
* @param s2 seconde chaîne
* 
* @return un entier strictement négatif si s1 est inférieur à s2, strictement
* 	 positif si s1 est supérieur à s2 et nul si s1 est identique à s2.
*/
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
