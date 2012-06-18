#include <stdio.h>

int fgetpos(FILE *stream, fpos_t *pos) {
	if (pos) {
		*pos = ftell(stream);
		return 0;
	}
	return -1;
}

int fsetpos(FILE *stream, fpos_t *pos) {
	if (pos) {
		return fseek(stream, (long)*pos, SEEK_SET);
	}
	return -1;
}
