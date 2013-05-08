/**
 * @file klibc/stdlib.h
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012 - TacOS developers.
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
 * @brief .
 */
#ifndef _STDLIB_H_
#define _STDLIB_H_

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
 *  L'entier base permet de spécifier la base à utiliser (decimal ou 
 *  hexadécimal).
 *
 * Converti l'entier d en une chaîne de caractère et le stock dans buf. Si 
 * base est égal à 'd', alors il interprète d comme étant en décimal et si 
 * base est égal à 'x', alors il interprète d comme étant en hexadécimal.
 * 
 * @param buf une chaîne de taille suffisament grande pour y stocker le 
 *  résultat de la transformation.
 * @param base la base à utiliser pour la conversion ('d' ou 'x').
 * @param d le nombre à convertir.
 */
void itoa(char *buf, int base, int d);

void itox(char *buf, int d);

#endif
