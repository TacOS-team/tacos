#ifndef _TTY_H
#define _TTY_H

#include "types.h"
#include "kfcntl.h"
#include "kprocess.h"

#define MAX_INPUT 255

typedef struct _terminal_t {
	bool echo;
	bool canon;
	// foreground_process; // Groupe de processus au premier plan (qui a le droit d'afficher des choses)
	process_t* fg_process;
	void (*put_char)(void*, char); // Fonction utilisée lors d'un affichage donc vers l'écran ou la ligne série.
	char buffer[MAX_INPUT];
	unsigned int p_begin;
	unsigned int p_end;
	void *extra_data;
	int sem;
} terminal_t;

void tty_init(terminal_t *term, void *extra_data, void (*put_char)(void*, char));

void tty_add_char(terminal_t *term, char c); // Fonction appelée par le driver clavier ou série pour ajouter un caractère.

size_t tty_write(open_file_descriptor *ofd, const void *buf, size_t count);
size_t tty_read(open_file_descriptor *ofd, void *buf, size_t count);

#endif
