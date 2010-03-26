#include <string.h>

void* memcpy(void* dest, const void* src, size_t size)
{
	size_t i;
	
	// Si on peut diviser les données à déplacer par paquet de 4 bytes, on les transfert en uint32, sinon on reste sur du uint8
	if(!(size%4))
	{
		uint32_t* d = dest;
		uint32_t* s = src;
		
		i = 0;
		while(i<(size/4))
		{
			d[i] = s[i++];
		}	
	}
	else
	{ 
		uint8_t* d = dest;
		uint8_t* s = src;
		
		i = 0;
		while(i<size)
		{
			d[i] = s[i++];
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
	int i = 0;
	while (s1[i] == s2[i]) {
		if (s1[i] == '\0' || i >= n) {
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
	int i = 0;
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
