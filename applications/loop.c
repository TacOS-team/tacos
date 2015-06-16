#include <unistd.h>

int main() {
	while (1) {
		write(1, "hello\n", 6);
		sleep(2);
	}
	return 0;
}
