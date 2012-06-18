/**
 * \file readline.h
 * \brief Lecture des lignes rentrées dans le shell
 * \author Bul0t Team
 * \date mai 2012
 */

#ifndef _READLINE_H_
#define _READLINE_H_
#include "shell.h"

/**
 * \fn void readline(shell_struct *shell)
 * \brief Fonction de lecture d'une ligne du terminal
 */
void readline(shell_struct *shell);

int getbigchar();

#endif
