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

char *fgets(char *s, int size, FILE *stream) {
	// TODO : Modifier fgetc pour le transformer et fgets puis changer fgetc en un appel à fgets. (juste une idée pour éviter de la redondance...)
	return s;
}

int fgetc(FILE *stream) {
	int c = EOF;
	char *i;
	
	char * pointeur;
	pointeur = stream->_IO_read_base;

	while(stream->_IO_read_base == NULL || (stream->_IO_read_ptr <= stream->_IO_buf_end && search_endl(stream->_IO_buf_base, stream->_IO_read_ptr) == NULL)) {
		if (stream->_IO_read_base != NULL && stream->_IO_read_ptr > pointeur && stream->_IO_read_ptr > stream->_IO_read_base) {
			if (pointeur == NULL) pointeur = stream->_IO_read_base;
			c = *pointeur;

			if (c == '\b') {
				if (pointeur > stream->_IO_read_base) {
					for (i = pointeur - 1; i < stream->_IO_read_ptr - 1; i++) {
						*i = *(i+2);
					}
					pointeur -= 2;
					stream->_IO_read_ptr -= 2;
				} else {
					for (i = pointeur; i < stream->_IO_read_ptr; i++) {
						*i = *(i+1);
					}
					stream->_IO_read_ptr -= 1;
					continue;
				}
			}

			if (stream->_fileno == 0) {
				printf("%c", c);
				fflush(stdout);
			}
			pointeur++;
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
