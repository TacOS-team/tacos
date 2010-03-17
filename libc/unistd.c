#include <unistd.h>
#include <types.h>
#include <events.h>

void* sleep_event(void* data)
{
	*((int*)data) = 0;
	return NULL;
}

unsigned int sleep(unsigned int seconds)
{
	return usleep(1000*seconds);
}

unsigned int usleep(unsigned int milliseconds)
{
	volatile int sleeping = 1;
	add_event(sleep_event, &sleeping, milliseconds);
	while(sleeping);
	
	return 0;
}
