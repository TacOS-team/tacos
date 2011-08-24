#include <stdio.h>

int main() {
	FILE* d = fopen("toto", "w+");
	fclose(d);
	return 0;
}
