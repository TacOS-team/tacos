#include <stdio.h>
#include <stdlib.h>
#include <process.h>

int main(int argc, char** argv)
{
	/* woot exec bomb */
	printf("1\n");
	printf("2\n");
	printf("3\n");
	printf("boom!");
	exec_elf("fd0:/bin/test");
	
	return 0;
}
