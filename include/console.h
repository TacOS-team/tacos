#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "types.h"

struct console_t {
	bool used;
	unsigned int n_page;
	unsigned int cur_x;
	unsigned int cur_y;
	bool disp_cur;
	char attr;
	unsigned int lines;
	unsigned int cols;
};

/**
 * @brief Passe une console au premier plan.
 *
 * @param n Numero de la console.
 */
void focus_console(int n);

/**
 * @brief Libère une console.
 *
 * @param n Numero de la console.
 */
void free_console(int n);

/**
 * @brief Initialise les consoles à "inutilisée".
 */
void init_console();

/**
 * @brief Trouve une console libre.
 */
int get_available_console();

/**
 * @brief Affiche un caractère. Supporte les caractères ANSI.
 *
 * @param n Numero de la console.
 * @param c Caractère à afficher.
 */
void kputchar(int n, char c);

#endif
