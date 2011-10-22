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
#include <sys/types.h>
#include <signal.h>

void handler(int signal) {
	printf("handler!\n");
}

void f1() {
	printf("F1 suspendu...\n");
	signal(SIGUSR1, handler);
	sigset_t set;
    sigfillset(&set);
    sigdelset(&set, SIGUSR1);
    sigsuspend(&set);
    printf("F1 réveillée!\n");
    while(1);
    
}
/* 0x4000269d */
void f2(pid_t pid) {
	printf("F2 tente de réveiller pid=%d...\n",pid);
	kill(pid, SIGUSR1);
}

int main(int argc, char** argv) {
	
	if(argc <= 1) {
		printf("Pas assez d'arguments\n\n");
		printf("Usage:\n");
		printf("\"test_sig 1\"\tprocess en attente de SIGUSR1\n");
		printf("\"test_sig 2 Pid\"\tEnvois un SIGUSR1 au process correpondant au PID\n");
		exit(0);
	}
	
	if(strcmp(argv[1],"1") == 0) {
		f1();
	} else if(strcmp(argv[1],"2") == 0) {
		f2(atoi(argv[2]));
	} else {
		printf("Mauvais paramètres\n");
	}
	while(1);
    return 0;
}


