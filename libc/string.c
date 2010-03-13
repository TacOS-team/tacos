#include <string.h>

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
