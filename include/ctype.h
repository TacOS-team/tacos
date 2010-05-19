#ifndef _CTYPE_H_
#define _CTYPE_H_

/**
 * @file ctype.h
 */


/** 
 * @brief vérifie si l'on a un caractère alphanumérique.
 * 
 * Vérifie si l'on a un caractère alphanumérique. C'est équivalent à 
 * (isalpha(c) || isdigit(c)).
 * 
 * @param c le caractère à vérifier.
 * 
 * @return vrai si le caractère est alphanumérique.
 */
int isalnum(int c);

/** 
 * @brief vérifie si l'on a un caractère alphabétique.
 * 
 * Vérifie si l'on a un caractère alphabétique. Attention, ici on suppose 
 * qu'un caractère alphabétique est soit minuscule soit majuscule et exclu 
 * donc la gestion d'une locale où un caractère alphabétique ne serait ni
 * minuscule ni majuscule.
 *
 * @param c le caractère à vérifier.
 * 
 * @return vrai si le caractère est alphabétique.
 */
int isalpha(int c);

/** 
 * @brief vérifie si le caractère est blanc, c'est-à-dire un espace ou une
 * tabulation.
 * 
 * Vérifie si le caractère est blanc, c'est-à-dire un espace ou une
 * tabulation.
 *
 * @param c le caractère à vérifier.
 * 
 * @return vrai si le caractère est blanc.
 */
int isblank(int c);

/** 
 * @brief vérifie si l'on a un caractère de contrôle.
 * 
 * Vérifie si l'on a un caractère de contrôle.
 *
 * @param c le caractère à vérifier.
 * 
 * @return vrai si le caractère est de contrôle.
 */
int iscntrl(int c);

/** 
 * @brief vérifie si l'on a un chiffre (0 à 9).
 * 
 * @param c le caractère à vérifier.
 * 
 * @return vrai si le caractère est un chiffre.
 */
int isdigit(int c);

/** 
 * @brief vérifie si l'on a un caractère minuscule.
 * 
 * @param c le caractère à vérifier.
 * 
 * @return vrai si le caractère est minuscule.
 */
int islower(int c);

/** 
 * @brief vérifie s'il s'agit d'un caractère imprimable, y compris l'espace.
 * 
 * @param c le caractère à vérifier.
 * 
 * @return vrai si le caractère est imprimable.
 */
int isprint(int c);

/** 
 * @brief vérifie s'il s'agit d'un caractère imprimable, qui ne soit ni un
 * espace, ni un caractère alphanumérique.
 * 
 * Vérifie s'il s'agit d'un caractère imprimable, qui ne soit ni un espace,
 * ni un caractère alphanumérique.
 *
 * @param c le caractère à vérifier.
 * 
 * @return vrai si le caractère est une ponctuation.
 */
int ispunct(int c);

/** 
 * @brief vérifie  si  l'on  a  un caractère blanc, d'espacement.
 * 
 * Vérifie  si  l'on  a  un caractère blanc, d'espacement.
 * Il s'agit de : espace, saut de page (form-feed, '\f'), saut de ligne 
 * (newline, '\n'), retour chariot (carriage return, '\r'), tabulation 
 * horizontale ('\t'), et tabulation verticale ('\v').
 *
 * @param c le caractère à vérifier.
 * 
 * @return vrai si le caractère est d'espacement.
 */
int isspace(int c);

/** 
 * @brief vérifie si l'on a un caractère majuscule.
 *
 * Vérifie si l'on a un caractère majuscule.
 * 
 * @param c le caractère à vérifier.
 * 
 * @return vrai si le caractère est majuscule.
 */
int isupper(int c);

/** 
 * @brief vérifie s'il s'agit d'un chiffre hexadécimal.
 *
 * Vérifie s'il s'agit d'un chiffre hexadécimal, c'est à dire
 * 0 1 2 3 4 5 6 7 8 9 a b c d e f A B C D E F.
 * 
 * @param c le caractère à vérifier.
 * 
 * @return vrai si le caractère est un chiffre hexadécimal.
 */
int isxdigit(int c);

/** 
 * @brief Convertit la lettre c en minuscule.
 *
 * Convertit la lettre c en minuscule si c'est possible.
 * 
 * @param c le caractère à convertir.
 * 
 * @return le caractère converti.
 */
int tolower(int c);

/** 
 * @brief Convertit la lettre c en majuscule.
 * 
 * Convertit la lettre c en majuscule si c'est possible.
 *
 * @param c le caractère à convertir.
 * 
 * @return le caractère converti.
 */
int toupper(int c);

#endif
