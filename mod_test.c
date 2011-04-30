#include "kernel/include/module.h"
#include "include/klog.h"

MODULE_NAME("Module de test");
MODULE_VERSION("0.1");

int test_func()
{
	klog("Test1");
}
int test_func2()
{
	klog("Test2");
}

int module_load()  
{
	klog("Loading module!!!1!");
	test_func();
	test_func2();
	return 0;
}


int module_unload()
{
	return 0;
}

MODULE_LOAD(module_load);
MODULE_UNLOAD(module_unload);

