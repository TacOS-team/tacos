#include <symtable.h>
#include <stdio.h>
#include <klog.h>

int init(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused)))
{
	init_stdfiles();
	klog("Starting init process...");
	
	klog("Loading kernel symbol table...");
	load_kernel_symtable();
	
	
	/* Initialisation des drivers */
	klog("Initializing drivers...");
	init_driver_list();
	init_dummy();
	init_mouse();
	/* ************************** */
	
	klog("Starting user process...");
	exec_elf("/bin/mishell", 0);
	
	while(1);
	return 0;
}
