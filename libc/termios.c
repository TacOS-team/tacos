#include <termios.h>
#include <sys/ioctl.h>

int tcgetattr(int fd, struct termios *termios_p) {
	ioctl(fd, 0, 0);
	// TODO.
	return 0;
}

int tcsetattr(int fd, int optional_actions, const struct termios *termios_p) {
	// TODO.
	return 0;
}
