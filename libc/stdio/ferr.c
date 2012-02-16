#include <libio.h>

void clearerr(FILE *stream) {
    stream->_flags &= ~(_IO_ERR_SEEN | _IO_EOF_SEEN);
}

int feof(FILE *stream) {
    return stream->_flags & _IO_EOF_SEEN;
}

int ferror(FILE *stream) {
    return stream->_flags & _IO_ERR_SEEN;
}

int fileno(FILE *stream) {
    return stream->_fileno;
}
