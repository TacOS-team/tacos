/**
 * \file command.h
 * \brief Gestion des commandes
 * \author Bul0t Team
 * \date mai 2012
 */


#include <termios.h>
#include "builtcommand.h"
#include "const.h"
#include "customs.h"
#include "jobs.h"
#include "shell.h"
#include "readline.h"

#include "arbre_yacc.h"
#include "y.tab.h"

#ifndef COMMAND_H_
#define COMMAND_H_

/**
 * \fn int readLine(char *buffer)
 * \brief Récupère une chaîne de longueur LENGTH_LINE depuis "stdin" et l'écrit dans "buffer" (et l'affiche à l'écran)
 * \param buffer buffer dans lequel on va écrire
 */
int readLine(char *buffer);

/**
 * \fn char* getCommand(char *buffer)
 * \brief Récupère une commande depuis un buffer
 * \param buffer buffer contenant la commande à récupérer
 */
char* getCommand(char *buffer);

/**
 * \fn void runCommand()
 * \brief Lance une commande
 */
void runCommand();


#endif /* COMMAND_H_ */
