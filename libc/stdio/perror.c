#include <errno.h>
#include <stdio.h>

void perror(const char *s) {
	fprintf(stderr, "%s : %d\n", s, errno);
	// TODO: Afficher message en fonction de errno.
}
