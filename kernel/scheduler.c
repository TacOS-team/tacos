#include <stdio.h>
#include <types.h>
#include <scheduler.h>
#include <process.h>

#define MAX_PROC 255

struct process process_list[MAX_PROC];
uint8_t num_proc;

void init_scheduler()
{
	num_proc = 0;
	
}

int add_process(paddr_t prog, uint32_t argc, uint8_t** argv)
{
	if(num_proc < MAX_PROC)
	{
		struct process new_proc;
		
		if(init_process(prog, argc, argv, &new_proc) != 0)
		{
			return 2;
		}

		num_proc++;		
		process_list[num_proc] = new_proc;
		
		return 0;
	}
	
	return 1;
}
