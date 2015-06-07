/**
 * @file sem_ping.c
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

#include <sys/sem.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
	int semid1 = semcreate(4);
	int semid2;
	pid_t pid = getpid();

	semid2 = semcreate(42);
	semP(semid1);
	
	unsigned int i = 0;

	while (1) {
		semP(semid2);
		for (i = 0; i < 0x005FFFFE; i++) {
		}
		printf("%d:Ping?\n",pid);
		semV(semid1);
	}
	return 0;
}
