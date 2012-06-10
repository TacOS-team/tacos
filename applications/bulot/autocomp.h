/**
 * \file autocomp.h
 * \brief Fonctions d'autocomplétion
 * \author Bul0t Team
 * \date mai 2012
 */

#ifndef AUTOCOM_
#define AUTOCOM_

#include "customs.h"

/**
 * \fn char *lastword(char *s, char *end, int *aft, char delimiter)
 * \brief Retourne le dernier mot d'une chaîne ou NULL si la chaîne vaut NULL*/
char *lastword(char *s, char *end, int *aft, char delimiter);

/**
 * \fn char* look_up_command(char *word, int nb, int afc)
 * \brief Cherche une à compléter une commande en utilisant "word"
*/
char* look_up_command(char *word, int nb, int afc);

#endif
