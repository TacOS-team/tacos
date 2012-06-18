#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

int ioctl(int d, unsigned int request, void* data)
{
	int res = d;
	syscall(SYS_IOCTL, (uint32_t) &res, request, (uint32_t) data);
	return res;
}
