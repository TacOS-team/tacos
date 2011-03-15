#include "kernel/include/module.h"
#include "kernel/include/kstdio.h"

MODULE_NAME("Module de test");
MODULE_VERSION("0.1");


static int a = 1;
static char c = 'a';
static char string[]="test";

int fibo(int depth)
{
	int i;
	int a,b,c;
	c=0;
	b=1;
	for(i=0; i<depth; i++)
	{
		a=b+c;
		c=b;
		b=a;
		kprintf("fibo(%d)=%d\n",i,a);
	}
}


void module_load()  
{
	fibo(10);
}


void module_unload()
{
	int a = 0;
	int b;
	a = a+1;
	b = a;
}

MODULE_LOAD(module_load);
MODULE_UNLOAD(module_unload);

