#include "include/syscall.h"

int ajouter(int a, int b)
{
	return a+b;
}

int main(int argc, char** argv)
{
	int a = 0; 
	int b = 1;
	while(a < 10)
		a=ajouter(a,b);
		
	syscall(SYS_EXIT,0,0,0);
	return a;
}
