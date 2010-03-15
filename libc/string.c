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
