/**
 * @file stdio/fclose.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2014 TacOS developers.
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
 * @brief Define "fclose" method
 */


#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

int fclose(FILE* stream)
{
	fflush(stream);
	if (stream->_IO_write_base != NULL) {
		free(stream->_IO_write_base);
	}
	close(stream->_fileno);
	
	if (__file_list == stream) {
		__file_list = stream->_chain;
	} else {
		FILE* aux = __file_list;
		while (aux != NULL && aux->_chain != stream) {
			aux = aux->_chain;
		}
		if (aux == NULL) {
			return -1;
		} else {
			aux->_chain = stream->_chain;
		}
	}

	free(stream);
	
	/* TODO:
	 * Upon  successful  completion  0  is  returned.  Otherwise, EOF is
     * returned and errno is set to indicate the error.  In either  case
     * any  further  access  (including another call to fclose()) to the
     * stream results in undefined behavior
     */
	return 0;
}

int fcloseall(void) {
	while (__file_list != NULL) {
		fclose(__file_list);
	}
	return 0;
}
