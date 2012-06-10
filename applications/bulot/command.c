/**

 * command.c
 * Description:
 *
 * Auteurs: Alexandre, Guillaume, Mathieu, Léo
 */

#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "command.h"

/*prototype des fonctions relatives au parser*/
int yyparse(void);
int yy_scan_string();

typedef struct yy_buffer_state *YY_BUFFER_STATE;

char* my_input_ptr;
char* my_input;
char* my_input_lim;

int readLine(char *buffer)
{
	if (!fgets(buffer, LENGTH_LINE, stdin)) {
		printf("\n");
	}
	return EXIT_SUCCESS;
}

char* getCommand(char *buffer)
{
	int i, taille = 0;
	char *command;
	for (i = 0; buffer[i] != '\n'; i++) {
		taille++;
	}
	command = cmalloc(sizeof(char) * (taille + 1));
	strncpy(command, buffer, taille + 1);
	command[taille] = '\0';
	return command;
}


void runFile(shell_struct *shell)
{
	if (shell->buf[0] != ' ')
	{
		my_input = shell->buf;
		my_input_ptr = my_input;
		my_input_lim = (char *)(my_input + strlen(my_input));
		yyparse();
		/*on vide le buffer*/
		strncpy(shell->buf, "\0", 1024);
	}
	/* TODO: free de command et de pipe_command de la fonction parseCommand*/
}



void runCommand()
{
	extern FILE *yyin;
	FILE *fd;

	  /* Lecture dans un fichier */
	if (shell.script_mode) {
		yyin = shell.fichier_script;
		fd = yyin;
		yyparse();
		fclose(fd);
		/*on vide le buffer*/
		strncpy(shell.buf, "\0", 1024);

	} else {
	    /* Lecture de l'entrée standard */
		readline(&shell);
		if ((shell.buf[0] != ';' && shell.buf[0] != '\n' && shell.buf[0] != '\0') || shell.script_mode)
		{
			my_input = shell.buf;
			my_input_ptr = my_input;
			my_input_lim = (char *)(my_input + strlen(my_input));

			yy_scan_string (strcat(shell.buf,";"));
			yyparse();
			//fclose(fd);
			/*on vide le buffer*/
			strncpy(shell.buf, "\0", 1024);
		}
		/* TODO: free de command et de pipe_command de la fonction parseCommand*/
	}
}
