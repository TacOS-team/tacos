#include <unistd.h>
#include <sys/mman.h>
#include <sys/syscall.h>

struct mmap_data {
	void *addr;
	size_t length;
	int prot;
	int flags;
	int fd;
	off_t offset;
};

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
	struct mmap_data data;
	data.addr = addr;
	data.length = length;
	data.prot = prot;
	data.flags = flags;
	data.fd = fd;
	data.offset = offset;

	void *ret;

	syscall(SYS_MMAP, (uint32_t) &data, (uint32_t) &ret, 0);

	return ret;
}

/*int munmap(void *addr, size_t length) {

}*/

