#include <unistd.h>
#include <events.h>
#include <stdio.h>
#include <syscall.h>

unsigned int sleep(unsigned int seconds)
{
	return usleep(1000000*seconds);
}

unsigned int usleep(unsigned int microseconds)
{
	syscall(SYS_SLEEP, microseconds, 0, 0);
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

int seek(int fd, long offset, int whence) {
	syscall(SYS_SEEK, fd, (uint32_t)(&offset), (uint32_t)(&whence));

	return whence;
}
