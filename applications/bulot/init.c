/*
 * init.c
 * Description:
 *
 * Auteurs: Alexandre, Guillaume, Mathieu, Léo
 */

#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "customs.h"
#include "init.h"
#include "historic.h"
#include "jobs.h"
#include "shell.h"
#include "alias.h"
#include "arbre_yacc.h"
#include "table_symboles.h"

void init_tag()
{
	FILE *motd = NULL;
	char buf[256];

	motd = fopen("../files/bulot410.ansi", "r");
	if (motd != NULL) {
		/*lecture du motd si présent*/
		while (fgets(buf, 256, motd) != NULL) {
			printf("%s", buf);
		}
		fclose(motd);
	} else {
		printf("\n-------------------------------------------------\n");
		printf("\tWelcome to bulOt-shell version 0.0\n");
		printf("\n-------------------------------------------------\n");
	}
}

void exit_tag()
{
#ifdef DEBUG
	printf("\n-------------------------------------------------\n");
	printf("\tSee you tomorrow or sooner\n");
	printf("\n-------------------------------------------------\n");
#endif
}

void exit_free()
{
	vider_table_symboles(table_symboles);
	//vider_liste_arbres(arbres);	// vide les arbres restants (il y en a toujours dès le départ qui reste)
}

int shellPrompt(int init) {
	char* currentDirectory = 0;
	char *out = malloc(128);
	char name[20];
	gethostname((char *) &name, 20);
	int size;
	if (shell.verbose_prompt) { /* prompt verbeux */
		if (strncmp(getcwd(currentDirectory, 1023), getenv("HOME"), strlen(getenv("HOME"))) == 0) { /* si on est dans le "HOME" */
			sprintf(out, "%s@%s:~%s@> ", getenv("USER"), name, getcwd(currentDirectory, 1023) + strlen(getenv("HOME")) * sizeof(char));
		} else {
			sprintf(out, "%s@%s:%s@> ", getenv("USER"), name,
					getcwd(currentDirectory, 1023));
		}
	} else {
		sprintf(out, "%s@%s:@> ", getenv("USER"), name);
	}
	if (init)
		printf("%s", out);
	size = strlen(out);
	fflush(stdout);
	free(out);
	return size;
}

char* init_buffer_line()
{
	return cmalloc((LENGTH_LINE + 1) * sizeof(char));
}


pid_t init_shell()
{
	/* See if we are running interactively. */
	shell.shell_terminal = STDIN_FILENO;
	shell.shell_is_interactive = isatty (shell.shell_terminal);
	shell.alias_list = init_alias();
	init_hist();	/* on crée un historique vide */

	if (shell.shell_is_interactive) {
		/* Loop until we are in the foreground. */
#ifdef FONCTIONS_AVANCEES
		while (tcgetpgrp (shell.shell_terminal) != (shell.shell_pgid = getpgrp ())) {
			kill (-shell.shell_pgid, SIGTTIN);
		}
#endif

		/* Ignore interactive and job-control signals.*/
		signal (SIGINT, SIG_IGN);
		signal (SIGQUIT, SIG_IGN);
		signal (SIGTSTP, SIG_IGN);
		signal (SIGTTIN, SIG_IGN);
		signal (SIGTTOU, SIG_IGN);
		/* signal (SIGCHLD, sigchld_handler_notif); */

		/* Put ourselves in our own process group. */
		shell.shell_pgid = getpid ();

#ifdef FONCTIONS_AVANCEES
		if (setpgid (shell.shell_pgid, shell.shell_pgid) < 0) {
			perror ("Couldn’t put the shell in its own process group");
			exit (1);
		}

		/* Grab control of the terminal. */
		tcsetpgrp (shell.shell_terminal, shell.shell_pgid);

		/* Save default terminal attributes for shell. */
		tcgetattr (shell.shell_terminal, &shell.shell_tmodes);
#endif
	}

	return shell.shell_pgid;
}
