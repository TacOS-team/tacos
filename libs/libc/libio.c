/**
 * @file libio.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2015 TacOS developers.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * @section DESCRIPTION
 *
 * Description de ce que fait le fichier
 */

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
