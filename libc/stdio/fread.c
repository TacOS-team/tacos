/**
 * @file fread.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012, 2013 - TacOS developers.
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

#include <stdio.h>

/**
 * The fread( ) function shall read into the array pointed to by ptr up to nitems elements whose size
 * is specified by size in bytes, from the stream pointed to by stream. For each object, size calls shall
 * be made to the fgetc( ) function and the results stored, in the order read, in an array of unsigned
 * char exactly overlaying the object. The file position indicator for the stream (if defined) shall be
 * advanced by the number of bytes successfully read. If an error occurs, the resulting value of the
 * file position indicator for the stream is unspecified. If a partial element is read, its value is
 * unspecified.
 */

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	uint32_t to_read = size * nmemb;
	
	char* buff = (char*) ptr;
	int character;
	
	while(to_read>0)
	{
		character = fgetc(stream);
		
		if(character < 0)
			break;
		
		*buff = (char) character;
		to_read--;
		buff++;
	}
	
	return nmemb - (to_read/size);
}
