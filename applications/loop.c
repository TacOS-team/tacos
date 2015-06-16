#include <stdio.h>
#include <unistd.h>

int main() {
	int i;
	for (i = 0; i < 10000; i++) {
		write(1, "hello\n", 6);
	}
	return 0;
}
