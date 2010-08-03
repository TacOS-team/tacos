#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
// TODO : Réécriture complète de la fonction.
// La gestion du backspace doit se faire au niveau supérieur (il n'y a que dans une "console" qu'on se sert du backspace). À voir comment gérer correctement la chose lors d'un scanf...
// Je ne suis pas sûr que ça soit normal d'avoir un flush ici...

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

	if (stream->_IO_read_base == stream->_IO_read_ptr) {
		if (read(stream->_fileno, stream->_IO_read_ptr, 1) >= 0) {
			stream->_IO_read_ptr++;
		} else {
			return EOF;
		}
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
