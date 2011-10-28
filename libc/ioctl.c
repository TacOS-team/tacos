#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

int ioctl(int d, unsigned int request, void* data)
{
	syscall(SYS_IOCTL, d, request, (uint32_t) data);
	return 0;
}
