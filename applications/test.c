/**
 * @file test.c
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
 * Programme de test à usage variable
 */

#include <stdio.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>

int main()
{
	int* t = mmap(NULL, 800000, PROT_READ , MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	//t[20000] = 42;
	printf("0x%x %d\n", t, t[20000]);

	int* t2 = mmap((void*)0x42000000, 800000, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	t2[20000] = 42;
	printf("0x%x %d\n", t2, t2[20000]);

	int fd = open("/tacos/README", O_RDONLY);
	char* t3 = mmap(NULL, 100, PROT_READ, MAP_PRIVATE | MAP_FILE, fd, 0);
	int i;
	for (i = 0; i < 100; i++) {
		printf("%c", t3[i]);
	}
	printf("\n");

	return 0;
}
