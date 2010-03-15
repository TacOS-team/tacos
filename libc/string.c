#include <string.h>

void* memcpy(void* dest, const void* src, size_t size)
{
	size_t i = 0;
	char* d = dest;
	char* s = src;
	
	while(i<size)
	{
		d[i] = s[i++];
	}
	return d;
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
