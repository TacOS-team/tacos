#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>

int ioctl(int d, unsigned int request, void* data)
{
	syscall(SYS_IOCTL, d, request, data);
	return 0;
}
