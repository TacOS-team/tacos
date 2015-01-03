#include <stdio.h>
#include <unistd.h>

int main(int argc, char ** argv) {
	int symbolic = 0;
	int marg = 1;
	if (argc > 1 && argv[1][0] == '-') {
		int i = 1;
		while (argv[1][i] != '\0') {
			switch (argv[1][i]) {
				case 's': symbolic = 1; break;
			}
			i++;
		}
		marg++;
	}

	if (argc <= marg + 1) {
		fprintf(stderr, "%s: missing file operand\n", argv[0]);
		return 1;
	}

	const char *target = argv[marg];
	const char *linkpath = argv[marg + 1];

	if (symbolic) {
		return symlink(target, linkpath);
	} else {
		// TODO
		// link(target, linkpath);
	}

	return 0;
}
