/**
 * \file builtcommand.h
 * \brief Commandes built-in du shell
 * \author Bul0t Team
 * \date mai 2012
 */

#ifndef BUILTCOMMAND_H_
#define BUILTCOMMAND_H_

/**
 * \fn void open_bulot(int i, int colonnes, int fall)
 * \brief affiche le bulot en ASCII-ART
 * \param i
 * \param colonnes
 * \param fall
 */
void open_bulot(int i, int colonnes, int fall);

void sl(void (*ptfct)(int, int, int), int fall);

void cours_bulot_cours (int i, int colonnes, int fall);

void print_file_90(char *file);

#endif
