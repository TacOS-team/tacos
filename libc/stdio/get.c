#include <stdio.h>

int fgetc(FILE *stream) {
	char *i;
	int c = EOF;
	while (1) {
		if (stream->_IO_read_base != NULL && stream->_IO_read_ptr > stream->_IO_read_base) {
			c = *stream->_IO_read_base;
			for (i = stream->_IO_read_base; i < stream->_IO_read_ptr; i++) {
				*i = *(i+1);
			}
			stream->_IO_read_ptr--;
			break;
		}
	}

	if (stream->_fileno == 0) {
		printf("%c", c);
		fflush(stdout);
	}
	return c;
}

int getchar(void) {
	return fgetc(stdin);
}
