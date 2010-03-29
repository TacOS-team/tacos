#include <stdio.h>
#include <stdlib.h>

int fflush(FILE *stream) {
	write(stream->_fileno, stream->_IO_write_base, stream->_IO_write_ptr - stream->_IO_write_base);
	stream->_IO_write_ptr = stream->_IO_write_base;
}

static char buf[1000];

int fputc(int c, FILE *stream) {
	char *ptr = stream->_IO_write_ptr;
	// Ajoute dans le buffer.
	//
	if (stream->_IO_write_ptr == NULL) {
	//	char * buf = malloc(1000);
		stream->_IO_buf_base = buf;
		stream->_IO_buf_end = buf + 1000;
		stream->_IO_write_base = stream->_IO_buf_base;
		stream->_IO_write_end = stream->_IO_buf_base;
		stream->_IO_write_ptr = stream->_IO_buf_base;
	}
	*(stream->_IO_write_ptr++) = c;
	// Consulte le comportement du buffer pour savoir s'il faut flusher :
	if (!(stream->_flags & (_IO_UNBUFFERED | _IO_LINE_BUF))) {
		// Fully buffered : on flush que si le buffer est full !

		// C'est full ?
		if (stream->_IO_write_ptr >= stream->_IO_buf_end) {
			fflush(stream);
		}
	} else if (stream->_flags & _IO_LINE_BUF ) {
		// Line buffered : on flush si on prend un \n ou si on arrive à la 
		// fin du buffer.

		// Il faut flusher ?
		if (stream->_IO_write_ptr >= stream->_IO_buf_end || c == '\n') {
			fflush(stream);
		}
	} else {
		// Non bufferisé.
		fflush(stream);
	}

}

int fputs(const char *s, FILE *stream) {
	while(*s != '\0') {
		fputc(*s++, stream);
	}
}

int putchar(int c) {
	fputc(c, stdout);
}

int puts(const char *s) {
	fputs(s, stdout);
}
