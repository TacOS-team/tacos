/**
 * @file ctype.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010 - TacOS developers.
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
 * @brief Fonctions de classification des caractères.
 */

#include <ctype.h>

int isalnum(int c) {
	return isalpha(c) || isdigit(c);
}

int isalpha(int c) {
	return isupper(c) || islower(c);
}

int isblank(int c) {
	return c == ' ' || c == '\t';
}

int iscntrl(int c) {
	return c <= 0x1F || c == 0x7F;
}

int isdigit(int c) {
	return c >= '0' && c <= '9';
}

int islower(int c) {
	return (c >= 'a' && c <= 'z'); 
}

int isprint(int c) {
	return ispunct(c) || isalnum(c) || c == ' ';
}

int ispunct(int c) {
	return (c >= '!' && c <= '\'') 
		|| (c >= '(' && c <= '/') 
		|| (c >= ':' && c <= '?')
		|| c == '@'
		|| (c >= '[' && c <= '_')
		|| c == '`'
		|| (c >= '{' && c <= '~');
}

int isspace(int c) {
	return c == ' ' || c == '\t' || c == '\f' || c == '\n' || c == '\r' || c == '\v';
}

int isupper(int c) {
	return (c >= 'A' && c <= 'Z');
}

int isxdigit(int c) {
	return isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

int tolower(int c) {
	if (isupper(c)) {
		return c + ('a' - 'A');
	}
  return c;
}

int toupper(int c) {
	if (islower(c)) {
		return c + ('A' - 'a');
	}
  return c;
}

