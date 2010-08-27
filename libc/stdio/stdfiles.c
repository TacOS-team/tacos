/**
 * @file stdfiles.c
 */

#include <types.h>
#include <libio.h>
#include <string.h>
#include <kmalloc.h>

#undef stdin
#undef stdout
#undef stderr

static FILE _IO_stdin = {_IO_MAGIC + _IO_LINE_BUF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, 0, 0};
static FILE _IO_stdout = {_IO_MAGIC + _IO_LINE_BUF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, &_IO_stdin, 1, 0};
static FILE _IO_stderr = {_IO_MAGIC + _IO_UNBUFFERED, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, &_IO_stdout, 2, 0};

FILE *stdin = & _IO_stdin;
FILE *stdout = & _IO_stdout;
FILE *stderr = & _IO_stderr;

void init_stdfiles(FILE ** _stdin, FILE ** _stdout, FILE ** _stderr) {
	/* Allocation memoire. */
	FILE * __stdin = (FILE*) malloc(sizeof(FILE));
	FILE * __stdout = (FILE*) malloc(sizeof(FILE));
	FILE * __stderr = (FILE*) malloc(sizeof(FILE));

	/* On y copie les valeurs par défaut */
	memcpy(__stdin, &_IO_stdin, sizeof(FILE));
	memcpy(__stdout, &_IO_stdout, sizeof(FILE));
	memcpy(__stderr, &_IO_stderr, sizeof(FILE));

	/* On fait l'affectation aux arguments. */
	*_stdin = __stdin;
	*_stdout = __stdout;
	*_stderr = __stderr;
}
