#include <termios.h>
#include <sys/ioctl.h>

int tcgetattr(int fd, struct termios *termios_p) {
	ioctl(fd, TCGETS, (void*)termios_p);
	return 0;
}

int tcsetattr(int fd, int optional_actions, const struct termios *termios_p) {
	ioctl(fd, TCSETS, (void*)termios_p);
	return 0;
}
