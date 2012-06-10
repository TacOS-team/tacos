/**
 * \file init.h
 * \brief Fonctions d'initialisation du shell
 * \author Bul0t Team
 * \date mai 2012
 */

#ifndef INIT_H_
#define INIT_H_

#include "shell.h"

/**
 * \fn void init_tag()
 * \brief Affiche le message de bienvenue
 */
void init_tag();

/**
 * \fn void exit_tag()
 * \brief Affiche le message d'adieu
 */
void exit_tag();

/**
 * \fn int shellPrompt(int opt)
 * \brief Affiche le prompt
 * \param init booléen indiquant si le prompt est affiché ou pas
 */
int shellPrompt(int init);

/**
 * \fn char* init_buffer_line()
 * \brief Alloue l'espace pour le buffer de ligne principale
 */
char* init_buffer_line();

/**
 * \fn pid_t init_shell()
 * \brief Effectue les principales initialisations (macro-variable "shell", historique, mise en foreground, gestion des signaux, ...)
 * \return Retourne le PGID du shell
 */
pid_t init_shell();


void exit_free();

#endif /* INIT_H_ */
