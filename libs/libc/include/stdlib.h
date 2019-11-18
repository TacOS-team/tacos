/**
 * @file libc/include/stdlib.h
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2015 TacOS developers.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * @section DESCRIPTION
 *
 * Description de ce que fait le fichier
 */

#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

#include <sys/types.h>
#include <malloc.h>

#define EXIT_SUCCESS 0 /**< Code retour en cas de succès. */
#define EXIT_FAILURE 1 /**< Code retour d'erreur. */

#define RAND_MAX 2147483647

/**
 * @brief Terminer de manière anormale un programme.
 */
void abort(void);

/**
 * @brief Modifie la taille du bloc de mémoire.
 *
 * @param ptr Le bloc mémoire à modifier.
 * @param size Nouvelle taille.
 *
 * @return nouvelle adresse.
 */
void *realloc(void *ptr, size_t size);

/**
 * @brief Alloue un bloc mémoire initiallement à 0.
 *
 * @param nmemb Nombre d'éléments.
 * @param size Taille d'un élément.
 *
 * @return Adresse du bloc alloué.
 */
void *calloc(size_t nmemb, size_t size);

long int strtol(const char* nptr, char** endptr, int base);
unsigned long int strtoul(const char *nptr, char **endptr, int base);
long long int strtoll(const char* nptr, char** endptr, int base);
unsigned long long int strtoull(const char *nptr, char **endptr, int base);

/**
 * Converti une chaine de caractère en entier.
 *
 * @param __nptr La chaine à traduire.
 *
 * @return La valeur convertie.
 */
int atoi(const char* __nptr);

/** 
* @brief Converti l'entier d en une chaîne de caractère et le stock dans buf.
*	 L'entier base permet de spécifier la base à utiliser (decimal ou 
*	 hexadécimal).
*
* Converti l'entier d en une chaîne de caractère et le stock dans buf. Si 
* base est égal à 'd', alors il interprète d comme étant en décimal et si 
* base est égal à 'x', alors il interprète d comme étant en hexadécimal.
* 
* @param buf une chaîne de taille suffisament grande pour y stocker le 
*	 résultat de la transformation.
* @param base la base à utiliser pour la conversion ('d' ou 'x').
* @param d le nombre à convertir.
*/
void itoa (char *buf, int base, int d);

void dtoa(double n, char *buf, int p);


int rand(void);
void srand(unsigned int seed);

void exit(int value);

void init_environ(char **envp);

char *getenv(const char *name);

int putenv(char *string);

int clearenv(void);

void qsort(void *base, size_t nmemb, size_t size, int(*compar)(const void *, const void *));

__END_DECLS

#endif

