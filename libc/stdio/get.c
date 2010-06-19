#include <stdio.h>

/**
 * @file get.c
 */

/** 
 * @brief Recherche un retour chariot ou une fin de buffer.
 * 
 * Cette fonction recherche un retour chariot dans un buffer en partant de la fin. 
 * C'est un peu plus efficace que de faire appel à des fonctions classiques.
 *
 * @param buf La chaine de caractère à analyser.
 * @param endbuf L'adresse de la fin du buffer.
 * 
 * @return l'adresse du retour chariot ou de la fin de fichier en cas de succès. NULL sinon.
 */
static char * search_endl(char * buf, char * endbuf) {
	endbuf--;
	while (endbuf >= buf) {
		if (*endbuf == '\n' || *endbuf == EOF) {
			return endbuf;
		}
		endbuf--;
	}
	return NULL;
}

char *fgets(char *s, int size, FILE *stream) {
	int i = 0;
	int c = 0;
	while (i < size - 1 && c != '\n') {
		c = fgetc(stream);
		if (c == EOF) { // Je pars du principe que le \n on le stock dans le buffer mais pas EOF.
			break;
		}
		s[i] = (char)c;
		i++;
	}
	s[i] = '\0'; // Termine la chaine par un \0.
	return s;
}


int fgetc(FILE *stream) {
	int c = EOF;
	char *i;
	
	char * pointeur;
	pointeur = stream->_IO_read_base;

	if (stream->_IO_read_ptr == NULL) {
      char * buf = (char*) malloc(1000);
      stream->_IO_buf_base = buf;
      stream->_IO_buf_end = buf + 1000;
      stream->_IO_read_base = buf;
      stream->_IO_read_end = buf;
      stream->_IO_read_ptr = buf;
	}

	read(stream->_fileno, stream->_IO_read_ptr, 1);
	stream->_IO_read_ptr++;

	while (stream->_IO_read_base == NULL || (stream->_IO_read_ptr <= stream->_IO_buf_end && search_endl(stream->_IO_buf_base, stream->_IO_read_ptr) == NULL)) {
		if (stream->_IO_read_base != NULL && stream->_IO_read_ptr > pointeur && stream->_IO_read_ptr > stream->_IO_read_base) {
			if (pointeur == NULL) pointeur = stream->_IO_read_base;
			c = *pointeur;

			if (c == '\b') {
				if (pointeur > stream->_IO_read_base) {
					for (i = pointeur - 1; i < stream->_IO_read_ptr - 1; i++) {
						*i = *(i+2);
					}
					pointeur -= 2;
					stream->_IO_read_ptr -= 2;
				} else {
					for (i = pointeur; i < stream->_IO_read_ptr; i++) {
						*i = *(i+1);
					}
					stream->_IO_read_ptr -= 1;
					continue;
				}
			}

			// Normalement il faut faire appel à la fonction read qui s'occupe de faire ce qu'il faut.
			// C'est à dire appeler la fonction de lecture/affichage associé au stream qui va bien !
			pointeur++;

			// Unbuffered => on flush direct
			if((stream->_flags & _IONBF) == _IONBF)
				break;
		}

		read(stream->_fileno, stream->_IO_read_ptr, 1);
		stream->_IO_read_ptr++;
	} 

	if (stream->_IO_read_base == stream->_IO_read_ptr) {
		return EOF;
	}

	c = *stream->_IO_read_base;
	for (i = stream->_IO_read_base; i < stream->_IO_read_ptr; i++) {
		*i = *(i+1);
	}

	stream->_IO_read_ptr--;
	return c;
}
/**
 * Fonction de la libc.
 * getchar est équivalent à fgetc(stdin).
 */
int getchar(void) {
	return fgetc(stdin);
}
