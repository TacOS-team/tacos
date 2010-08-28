#include <stdio.h>
#include <stdlib.h>
#include <process.h>

int main(int argc, char** argv)
{
	exec_elf("fd0:/bin/top");
	
	return 0;
}
