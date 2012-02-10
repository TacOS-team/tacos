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

//#define DEBUG

static int var;

static void usr_handler1(int signum __attribute__((unused)))
{
#ifdef DEBUG
	printf("usr_handler1 %d\n");
#endif
	var = 12345;
	return;
}

static void usr_handler2(int signum)
{
#ifdef DEBUG
	printf("usr_handler2 %d\n", signum);
#endif
	var = signum * 2;
	return;
}

static int f(int a) {
	if (a > 0) {
		return a * f(a-1);
	} else {
		return 0;
	}
}

static void usr_handler3(int signum)
{
	var = f(signum);
	return;
}

static void usr_handler4(int signum) {
	var *= signum;
	kill(getpid(), SIGUSR2);
}

static void usr_handler5(int signum) {
	var += signum;
}

static void usr_handler6(int signum __attribute__((unused))) {
	var += 5;
	sleep(3);
	var += 6;
}

static void usr_handler7(int signum __attribute__((unused))) {
	int a = time(NULL);
	sleep(3);
	var = time(NULL) - a;
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
		printf("Attendu : %d, Obtenu : %d\n", attendu, obtenu);
	}
}

int main()
{
	int pid = getpid();
	
	// Test 1 :
	signal(SIGUSR1, usr_handler1);
	var = 0;
	kill(pid, SIGUSR1);
	sleep(1);
	unit_test("Mise en place d'un handler et envoi d'un signal.", 12345, var);

	// Test 2 :
	signal(SIGUSR1, usr_handler2);
	var = 0;
	kill(pid, SIGUSR1);
	sleep(1);
	unit_test("Utilisation du numero du signal dans le handler.", 2 * SIGUSR1, var);

	// Test 3 :
	signal(SIGUSR1, SIG_IGN);
	var = 12;
	kill(pid, SIGUSR1);
	sleep(1);
	unit_test("Le handler remplacé par SIG_IGN.", 12, var);

	// Test 4 :
	signal(SIGUSR1, usr_handler3);
	var = 3;
	kill(pid, SIGUSR1);
	sleep(1);
	unit_test("Appels de fonction dans le handler.", f(SIGUSR1), var);

	// Test 5 :
	signal(SIGUSR1, usr_handler4);
	signal(SIGUSR2, usr_handler5);
	var = 2;
	kill(pid, SIGUSR1);
	sleep(1);
	unit_test("Envoi de signal depuis le handler d'un autre signal.", 2 * SIGUSR1 + SIGUSR2, var);

	// Test 6 (test à vérifier, peut-être un peu tendancieux) :
	signal(SIGUSR1, usr_handler6);
	var = 4;
	kill(pid, SIGUSR1);
	sleep(1);
	kill(pid, SIGUSR1);
	unit_test("Envoi d'un signal alors que le précédent n'a pas fini d'être traité.", 4 + 5 + 6 + 5 + 6, var);

	// Test 7 :
	signal(SIGUSR1, usr_handler7);
	var = 42;
	kill(pid, SIGUSR1);
	sleep(1);
	unit_test("Sleep de 3 secondes dans le handler alors que j'ai un sleep de 1s ici aussi.", 3, var);

	return 0;
}
