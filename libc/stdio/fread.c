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
	size_t i,j;
	for (i = 0; i < nmemb; i++) {
		unsigned char * element = &(((unsigned char *)ptr)[i*size]);
		
		for(j = 0; j < size; j++) {
			int ret = fgetc(stream);
			element[j] = ret;
			if (ret < 0) {
				return i;
			}
		}
	}

	return i;
}
