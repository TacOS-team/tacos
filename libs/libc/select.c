#include <sys/select.h>
#include <sys/syscall.h>
#include <unistd.h>

int select(uint32_t *fds, size_t n) {
	int ret;
	syscall(SYS_SELECT, (uint32_t) fds, (uint32_t) n, (uint32_t) &ret);
	return ret;
}
