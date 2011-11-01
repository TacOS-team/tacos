/**
 * @file matrix.c
 *
 * @author TacOS developers 
 *
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
 * Description de ce que fait le fichier
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc __attribute__ ((unused)), char* argv[] __attribute__ ((unused)))
{
	int i;
	printf("\033[40m\033[32m\033[0j");
	while(1) {
		int c1 = (int)(78.0*(float)rand()/(RAND_MAX+1.0));
		int c2 = (int)(78.0*(float)rand()/(RAND_MAX+1.0));
		char caractere1 = 33 + ((unsigned int)rand())%80;
		char caractere2 = 33 + ((unsigned int)rand())%80;
		if (c2 < c1) {
			int b = c1;
			c1 = c2;
			c2 = b - c2;
		} else {
			c2 -= c1;
		}

		while(c1--) printf(" ");
		printf("%c", caractere1);

		while(c2--) printf(" ");
		printf("%c\n", caractere2);

		// usleep plante, mais en même temps, il n'utilise même pas de syscall...
		//usleep(1000);
		for(i = 0; i < 100000; i++);
	}
}

