#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>

int rename(const char *oldpath, const char *newpath) {
	int ret;

	syscall(SYS_RENAME, (uint32_t)(oldpath), (uint32_t)(newpath), (uint32_t)&ret);

	return ret;
}
