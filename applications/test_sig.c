/**
 * @file test_sig.c
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
 * @brief Programme de test des signaux.
 */

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define DEBUG

static int var;

static void usr_handler1(int signum)
{
#ifdef DEBUG
	printf("usr_handler1 %d\n", signum);
#endif
	var = 12345;
	return;
}

static void padding(int c) {
	while (c-- > 0) {
		printf(" ");
	}
}

static void unit_test(const char* msg, int attendu, int obtenu) {
	int len = strlen(msg);
	printf("%s", msg);
	if (attendu == obtenu) {
		padding(80 - len - 4);
		printf("[OK]\n");
	} else {
		padding(80 - len - 7);
		printf("[ERROR]\n");
	}
}

int main()
{
	int pid = getpid();
	
	// Test 1 :
	signal(SIGUSR1, usr_handler1);
	var = 0;
	kill(pid, SIGUSR1);
	unit_test("Mise en place d'un handler et envoi d'un signal. ", 12345, var);

	return 0;
}
