#include <stdio.h>

static char * search_endl(char * buf, char * endbuf) {
	endbuf--;
	int i = 0;
	while (endbuf >= buf) {
		if (*endbuf == '\n') {
			return endbuf;
		}
		endbuf--;
	}
	return NULL;
}

int fgetc(FILE *stream) {
	int c = EOF;
	char *i;
	
	char * pointeur;
	pointeur = stream->_IO_read_base;

	while(stream->_IO_read_base == NULL || (stream->_IO_read_ptr <= stream->_IO_buf_end && search_endl(stream->_IO_buf_base, stream->_IO_read_ptr) == NULL)) {
		if (stream->_IO_read_base != NULL && stream->_IO_read_ptr > pointeur && stream->_IO_read_ptr > stream->_IO_read_base) {
			c = *(stream->_IO_read_ptr-1);

			if (c == '\b') {
				stream->_IO_read_ptr -= 2;
			}

			if (stream->_fileno == 0) {
				printf("%c", c);
				fflush(stdout);
			}
			pointeur = stream->_IO_read_ptr;
		}
	} 

	if (stream->_IO_read_base == stream->_IO_read_ptr) {
		return EOF;
	}

	c = *stream->_IO_read_base;
	for (i = stream->_IO_read_base; i < stream->_IO_read_ptr; i++) {
		*i = *(i+1);
	}

	stream->_IO_read_ptr--;
	return c;
}

int getchar(void) {
	return fgetc(stdin);
}
