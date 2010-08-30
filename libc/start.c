#include <process.h>
#include <libio.h>

typedef int main_type(int, char**);

extern main_type main;

void start_main()
{
	process_t *self = get_process(get_pid());
	
	init_process_malloc();
	
	init_stdfiles(&self->stdin, &self->stdout, &self->stderr);

	exit(main(0, NULL));
}
