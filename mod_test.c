#include "kernel/include/module.h"
#include "kernel/include/kstdio.h"

MODULE_NAME("Module de test");
MODULE_VERSION("0.1");


static int a = 1;
static char c = 'a';
static char string[]="test";

int func1(int c)
{
	return c+func2();
}

int func2()
{
	a++;
	c++;
	kprintf("%c %s\n",c,string);
	return a;
}


void module_load()  
{
	int a = 0;
	int b;
	kprintf("Hello world");
	b = func1(a);
	b = a;
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

