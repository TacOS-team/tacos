#include <stdio.h>

int main() {
	FILE* d = fopen("toto", "w+");
	fprintf(d, "Hello world\n");
	fclose(d);
	return 0;
}
