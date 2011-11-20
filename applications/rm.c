#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("usage: %s file1 [file2 file3...]", argv[0]);
	} else {
		int i;
		for (i = 1; i < argc; i++) {
			unlink(argv[1]);
		}
	}
	return 0;
}
