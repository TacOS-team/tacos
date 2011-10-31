#include <fcntl.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <vga_types.h>

char buffer[200][320];

struct mousedata {
	int x;
	int y;
	bool buttons[3];
};

int main() {
	int fd = open("$mouse", O_RDONLY);
	struct mousedata data;
	syscall(SYS_VGASETMODE, vga_mode_320x200x256, 0, 0);

	while (1) {
		read(fd, &data, sizeof(data));
		data.y = 200 - data.y;
		if (data.buttons[0]) {
			buffer[data.y][data.x] = 1;
		} else if (data.buttons[1]) {
			buffer[data.y][data.x] = 0;
		}
		char old = buffer[data.y][data.x];
		buffer[data.y][data.x] = 2;
		syscall(SYS_VGAWRITEBUF, (uint32_t)buffer, 0, 0);
		buffer[data.y][data.x] = old;
		usleep(10000);
	}
	close(fd);
	return 0;
}
