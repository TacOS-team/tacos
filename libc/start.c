#include <process.h>

typedef int main_type(int, char**);

extern main_type main;

void start_main()
{
	
	exit(main(0, NULL));
}
