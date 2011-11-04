#include <stdio.h>

extern char **environ;

int main() {
	if (environ != NULL) {
		int i = 0;
		while (environ[i] != NULL) {
			printf("%s\n", environ[i]);
			i++;
		}
	}
	return 0;
}
