/**
 * @file mishell.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012, 2013 - TacOS developers.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <shell_utils.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline.h>

void sigint_handler(int signal __attribute__((unused))) {
	printf("On ferme pas mishell!!\n");
}

void sigstp_handler(int signal __attribute__((unused))) {
}

static int pwd_cmd(int argc __attribute__((unused)), char **argv __attribute__((unused)))
{
	printf("%s\n", getcwd(NULL, 0));
	return 0;
}

static int help_cmd(int argc __attribute__((unused)), char **argv __attribute__((unused)))
{
	show_builtin_cmd();
	return 0;
}

static int cls_cmd() {
	printf("\033[2J");
	fflush(stdout);
	return 0;
}

static int cd_cmd(int argc, char **argv)
{
	char *buffer;
	if (argc == 1) {
		buffer = "/";
	} else if (argc == 2) {
		buffer = argv[1];
	} else {
		return 2;
	}
	if (chdir(buffer)) {
		printf("cd: aucun fichier ou dossier de ce type: %s\n", buffer);
		return 1;
	}
	return 0;
}

void print_logo()
{
	printf("_|_|_|_|_|                      _|_|      _|_|_|\n");
	printf("    _|      _|_|_|    _|_|_|  _|    _|  _|      \n");
	printf("    _|    _|    _|  _|        _|    _|    _|_|  \n");
	printf("    _|    _|    _|  _|        _|    _|        _|\n");
	printf("    _|      _|_|_|    _|_|_|    _|_|    _|_|_|    ");
	printf("(codename:fajitas)\n\n\n");
}

void exec(const char *buffer)
{
	if(exec_builtin_cmd(buffer) != 0)
	{
		int pid;
		if (buffer[0] == '/') {
			if ((pid = exec_elf(buffer)) < 0)
				printf("%s: commande introuvable.\n", buffer);
			else
				waitpid(pid);
		} else {
			char temp[278];
			char *path = getenv("PATH");
			sprintf(temp, "%s/%s", path, buffer);
			if ((pid = exec_elf(temp)) < 0)
				printf("%s: commande introuvable.\n", buffer);
			else
				waitpid(pid);
		}
	}
}

int main(int argc, char** argv)
{
	
	char *buffer;
	
	signal(SIGINT, sigint_handler);
	signal(SIGTSTP, sigstp_handler);
	signal(SIGTTOU, sigstp_handler);
	signal(SIGTTIN, sigstp_handler);

	add_builtin_cmd(help_cmd, "help");
	add_builtin_cmd(cls_cmd, "clear");
	add_builtin_cmd(pwd_cmd, "pwd");
	add_builtin_cmd(cd_cmd, "cd");

	if (argc > 1) {
		FILE* fd = fopen(argv[1], "r");
		char buf[200];

		if (fd != NULL) {
			while (fgets(buf, sizeof(buf), fd)) {
				size_t len = strlen(buf);
				if (len > 1) {
					buf[len - 1] = '\0';
					exec(buf);
				}
			}
			return 0;
		}

		return 1;
	}

	print_logo();

	for(;;)
	{
		//time_t curr_time = time(NULL);
		char prompt[256];
		sprintf(prompt, "%s>", getcwd(NULL, 0));
		
		//printf("%s", prompt);
		//fflush(stdout);
		//buffer = malloc(1000);
		//getline(buffer, 1000);
		buffer = readline(prompt);
		if (strlen(buffer) >= 1) {
			exec(buffer);
		}
		free(buffer);
	}
}
