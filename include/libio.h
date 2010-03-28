#ifndef _LIBIO_H_
#define _LIBIO_H_

#define _IO_MAGIC 0xFBAD0000 /* Magic number (je le garde par compatibilité) */
#define _IO_MAGIC_MASK 0xFFFF0000 /* Pour filter le magic number */
#define _IO_UNBUFFERED 2 
#define _IO_LINE_BUF 0x200 

struct _IO_FILE {
	int _flags;		/* High-order word is _IO_MAGIC; rest is flags. */

	/* The following pointers correspond to the C++ streambuf protocol. */
	/* Note:  Tk uses the _IO_read_ptr and _IO_read_end fields directly. */
	char* _IO_read_ptr;	/* Current read pointer */
	char* _IO_read_end;	/* End of get area. */
	char* _IO_read_base;	/* Start of putback+get area. */
	char* _IO_write_base;	/* Start of put area. */
	char* _IO_write_ptr;	/* Current put pointer. */
	char* _IO_write_end;	/* End of put area. */
	char* _IO_buf_base;	/* Start of reserve area. */
	char* _IO_buf_end;	/* End of reserve area. */
	/* The following fields are used to support backing up and undo. */
	char *_IO_save_base; /* Pointer to start of non-current get area. */
	char *_IO_backup_base;  /* Pointer to first valid character of backup area */
	char *_IO_save_end; /* Pointer to end of non-current get area. */

	struct _IO_FILE *_chain;

	int _fileno;
	int _flags2;
};

typedef struct _IO_FILE FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

#endif
