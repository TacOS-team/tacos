/**
 * @file include/string.h
 *
 * @author TacOS developers 
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
 * @brief Fonctions de manipulation de strings.
 */

#ifndef _STRING_H_
#define _STRING_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

#include <sys/types.h>

/** 
 * @brief Copie une zone mémoire.
 *
 * La fonction memcpy() copie une zone mémoire de size octets depuis src vers
 * la zone mémoire dest. Les deux zones ne doivent pas se chevaucher sinon il 
 * faut utiliser memmove.
 * 
 * @see memmove
 *
 * @param dest adresse mémoire où il faut copier
 * @param src adresse mémoire source
 * @param size taille de la zone à copier
 * 
 * @return un pointeur sur dest.
 */
void* memcpy(void* dest, const void* src, size_t size);

/** 
* @brief Calcule la longueur d'une chaîne de caractères.
*
* La fonction strlen() calcule la longueur de la chaîne de caractères s sans
* compter le caractère nul « \0 »
* 
* @param s la chaîne dont on veut calculer la longueur
* 
* @return le nombre de caractères dans la chaîne s.
*/
size_t strlen(const char* s) __attribute__ ((pure));

/**
 * @brief Rechercher un caractère dans une chaîne.
 *
 * La fonction strchr() renvoie un pointeur sur la première occurrence du
 * caractère c dans la chaîne s.
 *
 * @param s la chaîne dans laquelle on veut chercher le caractère.
 * @param c le caractère que l'on recherche dans la chaine.
 *
 * @return un pointeur sur la première occurence trouvée ou NULL si absent.
 */
char *strchr(const char *s, int c);

/**
 * @brief Rechercher un caractère dans une chaîne.
 *
 * La fonction strchr() renvoie un pointeur sur la dernière occurrence du
 * caractère c dans la chaîne s.
 *
 * @param s la chaîne dans laquelle on veut chercher le caractère.
 * @param c le caractère que l'on recherche dans la chaine.
 *
 * @return un pointeur sur la dernière occurence trouvée ou NULL si absent.
 */
char *strrchr(const char *s, int c);

/** 
* @brief Compare deux chaînes.
* 
* La fonction strcmp() compare deux chaînes de caractères et retourne en 
* résultat un nombre négatif, positif ou nul si s1 est respectivement
* inférieur, supérieur ou identique à s2.
* 
* @param s1 première chaîne
* @param s2 seconde chaîne
* 
* @return un entier strictement négatif si s1 est inférieur à s2, strictement
* 	 positif si s1 est supérieur à s2 et nul si s1 est identique à s2.
*/
int strcmp(const char *s1, const char *s2);

/** 
 * @brief Compare deux chaînes jusqu'à n caractères.
 * 
 * La fonction strncmp compare deux chaînes de caractères et retourne en 
 * résultat un nombre négatif, positif ou nul si les n premiers caractères
 * de s1 sont inférieurs, supérieurs ou identiques aux n premiers caractères
 * de s2.
 *
 * @param s1 première chaîne
 * @param s2 seconde chaîne
 * @param n nombre de caractères à comparer
 * 
 * @return un entier strictement négatif si les n premiers caractères de s1 
 * sont inférieurs aux n premiers caractères de s2, strictement positif s'ils
 * sont supérieurs et nul s'ils sont identiques.
 */
int strncmp(const char *s1, const char *s2, size_t n);


/** 
* @brief Compare deux chaînes.
* 
* La fonction strcasecmp() compare deux chaînes de caractères et retourne en 
* résultat un nombre négatif, positif ou nul si s1 est respectivement
* inférieur, supérieur ou identique à s2.
* 
* @param s1 première chaîne
* @param s2 seconde chaîne
* 
* @return un entier strictement négatif si s1 est inférieur à s2, strictement
* 	 positif si s1 est supérieur à s2 et nul si s1 est identique à s2.
*/
int strcasecmp(const char *s1, const char *s2);

/** 
 * @brief Compare deux chaînes jusqu'à n caractères.
 * 
 * La fonction strncasecmp compare deux chaînes de caractères et retourne en 
 * résultat un nombre négatif, positif ou nul si les n premiers caractères
 * de s1 sont inférieurs, supérieurs ou identiques aux n premiers caractères
 * de s2.
 *
 * @param s1 première chaîne
 * @param s2 seconde chaîne
 * @param n nombre de caractères à comparer
 * 
 * @return un entier strictement négatif si les n premiers caractères de s1 
 * sont inférieurs aux n premiers caractères de s2, strictement positif s'ils
 * sont supérieurs et nul s'ils sont identiques.
 */
int strncasecmp(const char *s1, const char *s2, size_t n);

/**
 * @brief Recherche un caractère dans une chaîne.
 *
 * Recherche un caractère dans une chaîne, et si le caractère n'est pas présent,
 * renvoie un pointeur vers l'octet nul de la chaîne.
 *
 * @param s La chaîne dans laquelle il faut effectuer une recherche.
 * @param c Le caractère à rechercher.
 *
 * @return Un pointeur sur la première occurrence du caractère dans la chaîne ou
 * un pointeur vers la caractère nul de la chaîne si le caractère n'est pas
 * trouvé.
 */
char *strchrnul(const char *s, int c);

/** 
 * @brief Rempli une zone mémoire avec un octet donné.
 * 
 * Rempli la zone mémoire pointée par s et de n octets par l'octet c.
 *
 * @param s zone mémoire à remplir
 * @param c octet qui sert pour le remplissage
 * @param n nombre d'octets à copier
 * 
 * @return un pointeur vers la zone mémoire s.
 */
void *memset(void *s, int c, size_t n) __attribute__ ((optimize ("no-tree-loop-distribute-patterns")));

/** 
 * @brief Compare deux zones mémoire.
 * 
 *	Compare les n premiers octets des zones mémoires s1 et s2.
 *	Elle retourne un entier négatif, positif ou nul si les n premiers octets de
 *	s1 sont respectivement plus petits, plus grands ou identiques aux n premiers 
 *	octets de s2.
 *
 * @see strcmp
 * @see strncmp
 *
 * @param s1 pointeur vers la première zone mémoire.
 * @param s2 pointeur vers la seconde zone mémoire.
 * @param n le nombre d'octets à comparer.
 * 
 * @return un entier négatif, positif ou nul si les n premiers octets de
 *	s1 sont respectivement plus petits, plus grands ou identiques aux n premiers 
 *	octets de s2.
 */
int memcmp(const void *s1, const void *s2, size_t n);

/** 
 * @brief Copie une chaine.
 *
 * Copie la chaine s2 vers la zone mémoire pointée par s1.
 * La copie s'arrête dès que le caractère '\0' est rencontré dans s2.
 * 
 * @param s1 zone mémoire de destination.
 * @param s2 chaine source.
 * 
 * @return un pointeur vers la chaîne de destination s1.
 */
char *strcpy(char * s1, const char * s2);


/** 
 * @brief Copie une chaine en se limitant aux n premiers caractères.
 * 
 * Copie la chaine s2 vers la zone mémoire pointée par s1 en s'arrêtant aux 
 * n premiers caractères de s1.
 * La copie s'arrête au bout de n caractères ou dès que le caractère '\0' 
 * est lu dans la chaine source s2. Attention, la chaîne s1 ne contient pas 
 * forcément le caractère '\0' si la limite des n caractères est atteinte.
 *
 * @param s1 zone mémoire de destination.
 * @param s2 chaine source.
 * @param n nombre de caractères à copier au maximum.
 * 
 * @return un pointeur vers la chaîne de destination s1.
 */
char *strncpy(char * s1, const char * s2, size_t n);


/** 
 * @brief Concaténation de deux chaînes.
 * 
 * Ajoute à la fin de s1 le contenu de s2 en s'arrêtant au premier caractère 
 * '\0' rencontré.
 *
 * @param s1 chaine destination.
 * @param s2 chaine source qui sera ajoutée à la fin de s1.
 * 
 * @return un pointeur vers la chaine de destination s1.
 */
char *strcat(char * s1, const char * s2);

/** 
 * @brief Concaténation de deux chaînes avec une limite aux n premiers 
 * caractères.
 * 
 * Ajoute à la fin de s1 le contenu de s2 en s'arrêtant à n caractères ou 
 * au premier caractère '\0' rencontré.
 *
 * @param s1 chaine destination.
 * @param s2 chaine source qui sera ajoutée à la fin de s1.
 * @param n nombre de caractères à copier.
 * 
 * @return un pointeur vers la chaine de destination s1.
 */
char *strncat(char * s1, const char * s2, size_t n);


/** 
 * @brief Copie une zone mémoire.
 * 
 * La fonction memmove() copie n octets depuis la zone mémoire src vers la 
 * zone mémoire dest. La différence avec memcpy c'est que les zones peuvent
 * se chevaucher.
 *
 * @see memcpy
 *
 * @param dest zone mémoire de destination.
 * @param src chaine source.
 * @param n nombre de caractères à copier au maximum.
 * 
 * @return un pointeur sur dest.
 */
void *memmove(void *dest, const void *src, size_t n);


/** 
 * @brief Recherche une sous-chaîne.
 *
 * Recherche needle (aiguille) dans la chaine heystack (meule de foin).
 * Les caractères « \0 » ne sont pas comparés.
 * 
 * @param haystack chaine dans laquelle on va effectuer la recherche.
 * @param needle sous-chaîne à rechercher.
 * 
 * @return un pointeur vers le début de la sous-chaîne ou NULL si non 
 * trouvée.
 */
char *strstr(const char *haystack, const char *needle);


/** 
 * @brief Recherche une sous-chaîne en ignorant la casse.
 * 
 * Recherche needle (aiguille) dans la chaine heystack (meule de foin).
 * Les caractères « \0 » ne sont pas comparés. La casse des arguments 
 * est ignorée.
 *
 * @param haystack chaîne dans laquelle on va effectuer la recherche.
 * @param needle sous-chaîne à rechercher.
 * 
 * @return un pointeur vers le début de la sous-chaîne ou NULL si non 
 * trouvée. 
 */
char *strcasestr(const char *haystack, const char *needle);

/** 
 * @brief Duplique une chaine.
 * 
 * strdup va faire un malloc de taille suffisante pour y copier la chaîne 
 * en paramètre.
 *
 * @param s la chaîne à dupliquer.
 * 
 * @return un pointeur vers la nouvelle zone mémoire contenant une copie de la chaîne.
 */
char *strdup (const char *s);

/** 
 * @brief Duplique une chaine.
 * 
 * strndup va faire un malloc de taille suffisante pour y copier la chaîne 
 * en paramètre en se limitant à n caractères au maximum.
 *
 * @param s la chaîne à dupliquer.
 * @param n la taille maximale de la chaîne.
 * 
 * @return un pointeur vers la nouvelle zone mémoire contenant une copie de la chaîne.
 */
char *strndup (const char *s, size_t n);



/**
 * @brief Extraire des mots d'une chaîne.
 *
 * @param str La chaine à tokenizer.
 * @param delim L'ensemble des délimiteurs possible.
 *
 * @return Un pointeur vers l'élément lexical suivant ou NULL s'il n'y en a
 * plus.
 */
char *strtok(char *str, const char *delim);


/* TODO: NOT IMPLEMENTED YET

int strcoll(const char *s1, const char *s2);

size_t strxfrm(char * s1, const char * s2, size_t n);

void *memchr(const void *s, int c, size_t n);

size_t strcspn(const char *s1, const char *s2);

char *strpbrk(const char *s1, const char *s2);

char *strrchr(const char *s, int c);

size_t strspn(const char *s1, const char *s2);

char *strerror(int errnum);

*/

__END_DECLS

#endif
