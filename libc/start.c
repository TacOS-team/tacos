#include <process.h>
#include <libio.h>

typedef int main_type(int, char**);

extern main_type main;

void start_main()
{
	init_process_malloc();
	
	init_stdfiles(&stdin, &stdout, &stderr);

	exit(main(0, NULL));
}
