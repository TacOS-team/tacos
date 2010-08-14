/**
 * @file stdio.c
 */

#include <stdio.h>
#include <stdlib.h>

int setvbuf(FILE *stream, char *buf, int mode, size_t size) {
  //kfree(stream->buffer);
	if(buf == NULL) {
		size = (mode == _IONBF) ? 1 : 512;
		stream->_IO_buf_base = malloc(size);
	} else {
		stream->_IO_buf_base = buf;
	}
	stream->_IO_buf_end = stream->_IO_buf_base + size;

	stream->_IO_read_base = stream->_IO_buf_base;
	stream->_IO_read_end = stream->_IO_buf_base;
	stream->_IO_read_ptr = stream->_IO_buf_base;
	stream->_IO_write_base = stream->_IO_buf_base;
	stream->_IO_write_end = stream->_IO_buf_base;
	stream->_IO_write_ptr = stream->_IO_buf_base;

	stream->_flags = mode;

	return 0;
}

