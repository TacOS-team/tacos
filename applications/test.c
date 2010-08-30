#include <stdio.h>
#include <stdlib.h>
#include <process.h>

int main(int argc, char** argv)
{
	int* ptr = malloc(10);
	printf("0x%x\n", ptr);
	
	ptr = malloc(10);
	printf("0x%x\n", ptr);

	ptr = malloc(10);
	printf("0x%x\n", ptr);

	ptr = malloc(10);
	printf("0x%x\n", ptr);

	ptr = malloc(10);
	printf("0x%x\n", ptr);

	ptr = malloc(10);
	printf("0x%x\n", ptr);

	ptr = malloc(10);
	printf("0x%x\n", ptr);
	
	ptr = malloc(10);
	printf("0x%x\n", ptr);
	free(ptr);
	
	return 0;
}
