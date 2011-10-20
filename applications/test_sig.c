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
/*
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <sys/types.h>
#include <unistd.h>

void int_handler(int sig)
{
	static int i = 0;
	printf("SIGINT recu(0x%x): i = %d.\n", sig,i);
	i++;
	if(i == 3)
		exit(0);
	return;
}
void usr_handler(int sig)
{
	printf("SIGUSR1 recu.(0x%x)\n", sig);
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
*/
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>

void handler1(int signal) {
    printf("[handler1] Activation du handler1 avec signal = %d\n", signal);
    printf("[handler1] => SIGUSR2\n");
    kill(getpid(), SIGUSR2);
		while(1);
    printf("[handler1] Fin exécution handler1.\n");
}

void handler2(int signal) {
    printf("[handler2] Activation du handler2 avec signal = %d\n", signal);
    printf("[handler2] => SIGUSR1\n");
	kill(getpid(), SIGUSR1);
    printf("[handler2] Fin exécution handler2.\n");
}

int main() {
    signal(SIGUSR1, handler1);
    signal(SIGUSR2, handler2);

    printf("[main] => SIGUSR1.\n");
    kill(getpid(), SIGUSR1);
    while(1);
    printf("[main] Fin du programme.\n");
    return 0;
}

