/**
 * @file string.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010 - TacOS developers.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * @section DESCRIPTION
 *
 * Description de ce que fait le fichier
 */

/**
 * @file string.c
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>


void* memcpy(void* dest, const void* src, size_t size)
{
	size_t i;
	
	// Si on peut diviser les données à déplacer par paquet de 4 bytes, on les transfert en uint32, sinon on reste sur du uint8
	if(!(size%4))
	{
		uint32_t* d = (uint32_t*) dest;
		uint32_t* s = (uint32_t*) src;
		
		i = 0;
		while(i<(size/4))
		{
			d[i] = s[i];
			i++;
		}	
	}
	else
	{ 
		uint8_t* d = (uint8_t*) dest;
		uint8_t* s = (uint8_t*) src;
		
		i = 0;
		while(i<size)
		{
			d[i] = s[i];
			i++;
		}
	}
	
	return dest;
}

size_t strlen(const char* s)
{
	int len=0;
	while(s[len++]);
	return len-1;
}
		
int strcmp(const char *s1, const char *s2) {
	int i = 0;
	while (s1[i] == s2[i]) {
		if (s1[i] == '\0') {
			return 0;
		}
		i++;
	}
	return s1[i] - s2[i];
}

int strncmp(const char *s1, const char *s2, size_t n) {
	unsigned int i = 0;
	while (s1[i] == s2[i]) {
		if (s1[i] == '\0' || i+1 == n) {
			return 0;
		}
		i++;
	}
	return s1[i] - s2[i];
}

void *memset(void *s, int c, size_t n)
{
	size_t i;
	
	for(i=0 ; i<n ; i++)
		((char*)s)[i] = (unsigned char)c;
		
	return s;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
	unsigned int i = 0;
	while (((char*)s1)[i] == ((char*)s2)[i]) {
		if (i >= n) {
			return 0;
		}
		i++;
	}
	return ((char*)s1)[i] - ((char*)s2)[i];
}

char *strcpy(char * s1, const char * s2)
{
	int i;
	for(i=0 ; s2[i] != '\0' ; i++)
	{
		s1[i] = s2[i];
	}
	s1[i] = '\0';
	
	return s1;
}

char *strncpy(char * s1, const char * s2, size_t n)
{
	size_t i;
	for(i=0 ; s2[i] != '\0' && i<n ; i++)
	{
		s1[i] = s2[i];
	}
	for(; i<n ; i++)
	{
		s1[i] = '\0';
	}
	
	return s1;
}

char *strcat(char * s1, const char * s2)
{
	strcpy(s1+strlen(s1), s2);
	return s1;
}

char *strncat(char * s1, const char * s2, size_t n)
{	
	if(n<strlen(s2))
		strncpy(s1+strlen(s1), s2, n);
	else
		strcpy(s1+strlen(s1), s2);
	
	return s1;
}

void *memmove(void *dest, const void *src, size_t n)
{
	if(dest > src)
	{
		int i;
		
		// Si on peut diviser les données à déplacer par paquet de 4 bytes, on les transfert en uint32, sinon on reste sur du uint8
		if(!(n%4))
		{
			uint32_t* d = dest;
			uint32_t* s = (uint32_t*) src;
			
			i =(int) (n/4)-1;
			while(i>=0)
			{
				d[i] = s[i];
				i--;
			}	
		}
		else
		{ 
			uint8_t* d = dest;
			uint8_t* s = (uint8_t*) src;
			
			i = n-1;
			while(i>=0)
			{
				d[i] = s[i];
				i--;
			}
		}
	}
	else
	{
		size_t i;
	
		// Si on peut diviser les données à déplacer par paquet de 4 bytes, on les transfert en uint32, sinon on reste sur du uint8
		if(!(n%4))
		{
			uint32_t* d = dest;
			uint32_t* s = (uint32_t*) src;
			
			i = 0;
			while(i<(n/4))
			{
				d[i] = s[i];
				i++;
			}	
		}
		else
		{ 
			uint8_t* d = dest;
			uint8_t* s = (uint8_t*) src;
			
			i = 0;
			while(i<n)
			{
				d[i] = s[i];
				i++;
			}
		}	
	}
	
	return dest;
}

char *strstr(const char *haystack, const char *needle)
{
	bool found = false;
	int i;
	
	// tant qu'on a pas trouvé et que c'est pas fini
	while(*haystack != '\0' && !found)
	{
		found = true;
		for(i=0 ; haystack[i]!='\0' && needle[i]!='\0' ; i++)
		{
			if(haystack[i] != needle[i])
			{
				found = false;
				break;
			}
		}
		if (needle[i] != '\0' && haystack[i] == '\0') {
			found = false;
		}
		
		haystack++;
	}
	
	if (found) {
		return (char*)haystack;
	}
	
	return NULL;
}

char *strcasestr(const char *haystack, const char *needle)
{
	bool found = false;
	int i;
	
	// tant qu'on a pas trouvé et que c'est pas fini
	while(*haystack != '\0' && !found)
	{
		found = true;
		for(i=0 ; haystack[i]!='\0' && needle[i]!='\0' ; i++)
		{
			if(tolower(haystack[i]) != tolower(needle[i]))
			{
				found = false;
				break;
			}
		}
		if (needle[i] != '\0' && haystack[i] == '\0') {
			found = false;
		}
		
		haystack++;
	}
	
	if (found) {
		return (char*)haystack;
	}
	
	return NULL;
}

char *strdup (const char *s)
{
	int len = strlen(s);
	char* new_string = (char*) malloc((len+1)*sizeof(char)); // len + 1 pour le '\0'
	
	return strcpy(new_string, s);
}

char *strchr(const char* s, int c)
{
	/* On parcourt la chaine */
	while(*s != '\0' || *s == c) /* le *s == c permet de considérer le cas où c est le caractère null */
	{
		if(*s == c) { /* Si le caractère est détecté, on renvoie le pointeur. */
			return (char *)s;
		}
		s++;
	}

	return NULL;
}

char *strrchr(const char* s, int c)
{
	char * ret = NULL;
	/* On parcourt la chaine */
	while(*s != '\0' || *s == c) /* le *s == c permet de considérer le cas où c est le caractère null */
	{
		if(*s == c) { /* Si le caractère est détecté, on renvoie le pointeur. */
			ret = (char *)s;
		}
		s++;
	}

	return ret;
}
	
char *strtok(char *str, const char *delim) {
	static char *current_str;
	char *ret;
	if (str != NULL) {
		current_str = str;
	}

	if (current_str == NULL) {
		return NULL;
	}

	int j = 0;
	while (delim[j] != '\0') {
		if (current_str[0] == delim[j]) {
			j = 0;
			current_str++;
			continue;
		}
		j++;
	}

	int i = 0;
	while (current_str[i] != '\0') {
		int j = 0;
		while (delim[j] != '\0') {
			if (current_str[i] == delim[j]) {
				current_str[i] = '\0';
				ret = current_str;
				current_str = &(current_str[i+1]);
				return ret;
			}
			j++;
		}
		i++;
	}

	ret = current_str;
	current_str = NULL;

	return ret;
}
