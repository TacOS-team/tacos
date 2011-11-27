#include <stdio.h>

int main() {
	FILE* f = fopen("/dev/ttyS0", "w+");
	fprintf(f, "Hello World!\n");
	fclose(f);

	f = fopen("/dev/ttyS0", "r");
	char buf[10];
	fread(buf, sizeof(char), 10, f);
	printf("%s\n", buf);
	fclose(f);
	return 0;
}
