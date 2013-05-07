/**
 * @file getline.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012, 2013 - TacOS developers.
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
#include <stdio.h>

int fgetline(FILE *fp, char s[], int lim) {
	char *t;
	int c;

	t = s;
	while (--lim>1 && (c=fgetc(fp)) != EOF && c != '\n') {
		*s++ = c;
	}
	/*if (c == '\n')
		*s++ = c;
	else */
	if (lim == 1) {
		*s++ = '\n';
		fprintf(stderr, "WARNING. fgetline: Line too long, splitted.\n");
	}
	*s = '\0';
	return s - t;
}


int getline(char *s, int lim) {
	return fgetline(stdin, s, lim);
}
