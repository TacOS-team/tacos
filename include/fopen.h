#ifndef _FOPEN_H_
#define _FOPEN_H_

/**
 * @file fopen.h
 */

/** 
 * @brief Conversion de modes de lecture/ecriture/ajout...
 *
 * Converti depuis la forme "r+" par exemple en un entier 
 * formé de la superposition de plusieurs masques.
 * 
 * @param mode le mode présenté sous la forme d'une chaine de caractère.
 * 
 * @return un entier contenant les mêmes informations.
 */
int convert_flags(const char *mode);

#endif
