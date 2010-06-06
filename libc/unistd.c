#include <unistd.h>
#include <types.h>
#include <events.h>
#include <process.h>
#include <stdio.h>
#include <libio.h>
#include <syscall.h>

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
	add_event(sleep_event,(void*) &sleeping, milliseconds);
	while(sleeping) {
		asm("hlt");
	}
	
	return 0;
}

void* sys_write(uint32_t fd, uint32_t p_buf, uint32_t count) {
	process_t * process = get_current_process();
	const void *buf = (const void*)p_buf;

	open_file_descriptor *ofd;

	if (process->fd[fd].used) {
		ofd = process->fd[fd].ofd;

		ofd->write(ofd, buf, count);
	}

	return NULL;
}

size_t write(int fd, const void *buf, size_t count) {
	syscall(SYS_WRITE, fd, (uint32_t) buf, count);

	return 0;
}
