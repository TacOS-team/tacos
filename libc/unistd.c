#include <unistd.h>
#include <types.h>
#include <events.h>
#include <process.h>
#include <stdio.h>
#include <libio.h>
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
void* sys_read(uint32_t fd, uint32_t p_buf, uint32_t count) {
	process_t * process = get_current_process();
	void *buf = (void*)p_buf;

	open_file_descriptor *ofd;

	if (process->fd[fd].used) {
		ofd = process->fd[fd].ofd;

		return ofd->read(ofd, buf, count);
	}

	return NULL;
}

size_t read(int fd, void *buf, size_t count) {
	syscall(SYS_READ, fd, (uint32_t) buf, count);

	// FIXME: Ok, ça risque de poser problème... :D. Read doit retourner le nombre d'octets lus !!!
	return 0;
}
