#include <unistd.h>
#include <types.h>
#include <events.h>
#include <stdio.h>
#include <syscall.h>

unsigned int sleep(unsigned int seconds)
{
	return usleep(1000*seconds);
}

unsigned int usleep(unsigned int milliseconds)
{
	syscall(SYS_SLEEP, milliseconds, 0, 0);
	return 0;
}

ssize_t write(int fd, const void *buf, size_t count) {
	syscall(SYS_WRITE, fd, (uint32_t) buf, (uint32_t)(&count));

	return count;
}

ssize_t read(int fd, void *buf, size_t count) {
	syscall(SYS_READ, fd, (uint32_t) buf, (uint32_t)(&count));

	return count;
}
