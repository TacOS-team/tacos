#include <kprocess.h>
#include <types.h>
#include <libio.h>
#include <kfcntl.h>

void sys_write(uint32_t fd, uint32_t p_buf, uint32_t count) {
	process_t * process = get_current_process();
	const void *buf = (const void*)p_buf;
	size_t *c = (size_t*)count;
	ssize_t *t = (ssize_t*)count;

	open_file_descriptor *ofd;

	if (process->fd[fd].used) {
		ofd = process->fd[fd].ofd;

		*t = ofd->write(ofd, buf, *c);
	}
}

void sys_read(uint32_t fd, uint32_t p_buf, uint32_t count) {
	process_t * process = get_current_process();
	void *buf = (void*)p_buf;
	size_t *c = (size_t*)count;
	ssize_t *t = (ssize_t*)count;

	open_file_descriptor *ofd;

	if (process->fd[fd].used) {
		ofd = process->fd[fd].ofd;

		*t = ofd->read(ofd, buf, *c);
	}
}


