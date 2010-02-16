#include <stdio.h>
#include <types.h>
#include <scheduler.h>

typedef int (*main_func_type) (uint32_t, uint8_t*);

typedef struct s_process
{
	
} process;

void add_process(paddr_t prog, uint32_t argc, uint8_t* argv)
{
	int (*p)(uint32_t, uint8_t*) = (main_func_type)prog;
	
	p(argc,argv);
}
