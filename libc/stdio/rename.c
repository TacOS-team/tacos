#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>

int rename(const char *oldpath, const char *newpath) {
	int ret;
	char *old = NULL;
	char *new = NULL;

	if (oldpath[0] != '/') {
		old = get_absolute_path(oldpath);
	}

	if (newpath[0] != '/') {
		new = get_absolute_path(newpath);
	}

	syscall(SYS_RENAME, (uint32_t)(old ? old : oldpath), (uint32_t)(new ? new : newpath), (uint32_t)&ret);

	if (old) free(old);
	if (new) free(new);
	return ret;
}
