/**
 * @file test_sig.c
 *
 * @author TacOS developers 
 *
 * Maxime Cheramy <maxime81@gmail.com>
 * Nicolas Floquet <nicolasfloquet@gmail.com>
 * Benjamin Hautbois <bhautboi@gmail.com>
 * Ludovic Rigal <ludovic.rigal@gmail.com>
 * Simon Vernhes <simon@vernhes.eu>
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
 * Programme de test pour les signaux
 */

#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <sys/types.h>
#include <unistd.h>

void int_handler()
{
	printf("SIGINT recu.\n");
	return;
}
void usr_handler()
{
	printf("SIGUSR1 recu.\n");
	return;
}

int main()
{
	pid_t pid = getpid();
	
	signal(SIGINT, int_handler);
	signal(SIGUSR1, usr_handler);
	
	kill(pid, SIGUSR1);
	while(1);
	return 0;
}
