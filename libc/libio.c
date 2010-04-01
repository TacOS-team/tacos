#include <libio.h>
#include <stdio.h>

void print_file(FILE *file) {
  printf("buffer : %x %s dd\n", file->_flags, !(file->_flags & (_IO_UNBUFFERED | _IO_LINE_BUF)) ? "full" : (file->_flags & _IO_LINE_BUF) ? "line" : "none");

  printf("_IO_read_ptr;  : %x\n", file->_IO_read_ptr);
  printf("_IO_read_end;  : %x\n", file->_IO_read_end);
  printf("_IO_read_base; : %x\n", file->_IO_read_base);
  printf("_IO_write_base; : %x\n", file->_IO_write_base);
  printf("_IO_write_ptr;  : %x\n", file->_IO_write_ptr);
  printf("_IO_write_end;  : %x\n", file->_IO_write_end);
  printf("_IO_buf_base;   : %x\n", file->_IO_buf_base );
  printf("_IO_buf_end;  : %x\n", file->_IO_buf_end);
  printf("_IO_save_base;  : %x\n", file->_IO_save_base);
  printf("_IO_backup_base; : %x\n", file->_IO_backup_base);
  printf("_IO_save_end; : %x\n", file->_IO_save_end);
}
