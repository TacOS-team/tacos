/**
 * @file test.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2014 TacOS developers.
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
 * Programme de test à usage variable
 */

#include <stdio.h>
#include <signal.h>
#include <sys/mman.h>

int main()
{
	int* t = mmap(0, 800000, 0, 0, -1, 0);
	t[20000] = 42;
	printf("%u %d\n", t, t[20000]);

	int* t2 = mmap(0, 800000, 0, 0, -1, 0);
	t2[20000] = 42;
	printf("%u %d\n", t2, t2[20000]);

	return 0;
}	
