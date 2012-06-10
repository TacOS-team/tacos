#include <errno.h>
#include <stdio.h>

void perror(const char *s) {
	fprintf(stderr, "%s : %s\n", s, sys_errlist[errno]);
}
