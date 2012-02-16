#include <unistd.h>
#include <sys/stat.h>

int remove(const char *pathname) {
	struct stat buf;
	stat(pathname, &buf);
	if (S_ISDIR(buf.st_mode)) {
		return rmdir(pathname);
	} else {
		return unlink(pathname);
	}
}
