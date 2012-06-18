/**
 * \file historic.h
 * \brief Gestion de l'historique du shell
 * \author Bul0t Team
 * \date mai 2012
 */

#ifndef HIST_H
#define HIST_H

#include "shell.h"

/**
 * \fn void init_hist()
 * \brief Permet d'initialiser un historique
 */
void init_hist();

/**
 * \fn void delete_hist()
 * \brief Vide l'historique (libère la mémoire proprement) puis lance init_hist() pour revenir à l'état initial
 */
void delete_hist();

/**
 * \fn void insert_new_cmd(char * cmd)
 * \brief Insère une nouvelle commande dans l'historique
 * \param cmd Nouvelle commande à intégrer
 */
void insert_new_cmd(char * cmd);

/**
 * \fn char * get_current_cmd()
 * \brief Renvoie la commande pointée actuellement
 */
char * get_current_cmd();

/**
 * \fn char * get_next_cmd()
 * \brief Renvoie la commande suivante dans l'historique
 */
char * get_next_cmd();

/**
 * \fn char * get_prev_cmd()
 * \brief Renvoie la commande précedente dans l'historique
 */
char * get_prev_cmd();

/**
 * \fn void reset_actual_ptr()
 * \brief Met le pointeur actuel sur la dernière commande
 */
void reset_actual_ptr();

/**
 * \fn int is_empty_hist()
 * \brief Permet de savoir si l'historique est vide
 * \return Renvoie 1 si l'historique est vide, 0 sinon
 */
int is_empty_hist();

/**
 * \fn print_hist()
 * \brief Affiche toute l'historique des commandes dans le terminal, appelé par la commande built-in : hist
 */
void print_hist();

/**
 * \fn char **search_cmd_in_hist(char *cmd)
 * \brief Renvoie une liste de commandes possibles de l'historique avec l'entrée "cmd" comme mot-clé
 * \param cmd_in chaîne de caractères à chercher dans les commandes
 * \return Retourne un pointeur char ** sur un tableau de commandes
 */
char **search_cmd_in_hist(char *cmd_in);

#endif
