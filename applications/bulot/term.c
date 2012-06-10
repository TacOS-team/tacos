/*
 * term.c
 * Description:
 *
 * Auteurs: Alexandre, Guillaume, Mathieu, Léo
 */

#include "term.h"

int save_termios (struct termios *save)
{
	return tcgetattr(STDIN_FILENO, save);
}

int restore_termios (struct termios *old)
{
	return tcsetattr(STDIN_FILENO, TCSANOW, old);
}

