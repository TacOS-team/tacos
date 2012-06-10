 /*
 * main.c
 * Description:
 *
 * Auteurs: Alexandre, Guillaume, Mathieu, Léo
 */

#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#include "builtcommand.h"
#include "command.h"
#include "const.h"
#include "init.h"
#include "jobs.h"
#include "shell.h"
#include "term.h"

shell_struct shell; /* Déclarée dans shell.h */

/**
 *
 * @param argc
 * @param argv
 * @param envp
 */
int main(int argc, char **argv, char **envp)
{
	struct termios save;
	int out = EXIT_SUCCESS;
	shell.end_b = 0;  /* Défini en externe dans init.h*/
	shell.verbose_prompt = 0; /* voir shell.h */
	/*modif du terminal*/
	save_termios(&save);
	init_shell();

	/* Lecture du bulot.rc */
	if ((shell.fichier_script = fopen("bulot.rc", "r+")) != NULL) {
		shell.script_mode = 1;
		runCommand();
	}

	switch (argc){
	case 3:
		if (strncmp(argv[1], "-x", 2) == 0) {
			shell.script_mode = 1;
			shell.fichier_script = fopen(argv[2], "r");
			runCommand();
			sigchld_handler_notif();
			exit_free();
			exit_tag();
		} else {
			perror("option inconnue\n");
		}
		break;

	case 1:
		printf("interactive mode, %d\n", (int)getpid());
		shell.script_mode = 0;
		init_tag();
		while (!shell.end_b) {
			sigchld_handler_notif();
			shellPrompt(1);
			runCommand();
		}
		exit_free();
		//exit_tag();
		break;
	default:
		out = EXIT_FAILURE;
		break;
	}

	restore_termios(&save);
	return out;
}
