/**
 * \file customs.h
 * \brief Fonctions utiles pour allouer/désallouer la mémoire avec messages d'erreurs
 * \author Bul0t Team
 * \date mai 2012
 */

#ifndef CUSTOM_H_
#define CUSTOM_H_

#include "const.h"

/**
 * \fn void *cmalloc(size_t size)
 * \brief Malloc avec messages d'erreurs
 * \param size taille d'allocation en octets
 */
void *cmalloc(size_t size);

/**
 * \fn void cfree(void *p)
 * \brief Free avec messages d'erreurs
 * \param p zone mémoire à désallouer
 */
void cfree(void *p);


#endif
