/**
 * @file mishell.c
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
 * Description de ce que fait le fichier
 */

#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <shell_utils.h>
#include <unistd.h>

int shell(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused)));

static int pwd_cmd()
{
	printf("%s", getcwd(NULL, 0));
	return 0;
}

static int help_cmd()
{
	show_builtin_cmd();
	return 0;
}

static int date_cmd()
{
	time_t curr_time = time(NULL);
	printf("%s",ctime(&curr_time));	
	return 0;
}

static int kill_cmd()
{
	int pid;
	scanf("%d",&pid);
	kill(pid, SIGKILL);
	return 0;
}

static int cls_cmd() {
	printf("\033[2J");
	fflush(stdout);
	return 0;
}

static int cd_cmd()
{
	char buffer[256];
	scanf("%s", buffer);
	if (chdir(buffer)) {
		printf("cd: aucun fichier ou dossier de ce type: %s", buffer);
		return 1;
	}
	return 0;
}

int ps()
{
	process_t* aux = get_process_list(FIRST_PROCESS);
	const int clk_per_ms = CLOCKS_PER_SEC / 1000;
	long int ms;
	int s;
	int m;
	int h;

	printf("pid\tppid\tname\t\ttime\t\t%CPU\tstate\n");   
	while(aux!=NULL)
	{
			
			/* Calcul du temps d'execution du processus */
			ms = aux->user_time / clk_per_ms;
			s = ms / 1000;
			
			m = s / 60;
			s = s % 60;
			h = m / 60;
			m = m % 60;
			
			/*if (aux->process == active_process) {
					printf("*");
			}*/
			
			printf("%d\t%d\t%s\t\t%dh %dm %ds\t%d\%\t",aux->pid, aux->ppid, aux->name, h, m ,s, (int)(((float)aux->last_sample/(float)CPU_USAGE_SAMPLE_RATE)*100.f));
			
			switch(aux->state)
			{
					case PROCSTATE_IDLE:
							printf("IDLE\n");
							break;
					case PROCSTATE_RUNNING:
							printf("RUNNING\n");
							break;
					case PROCSTATE_WAITING:
							printf("WAITING\n");
							break;
					case PROCSTATE_TERMINATED:
							printf("TERMINATED\n");
							break;
					default:
							break;
			}
			
			aux = get_process_list(NEXT_PROCESS);
	}
	return 0;
}

static int shell_exec_elf()
{
	char filename[80];
	scanf("%s",filename);
	return exec_elf(filename, 0);
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

int main(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused)))
{
	
	char buffer[256];

	add_builtin_cmd(help_cmd, "help");
	add_builtin_cmd(date_cmd, "date");
	add_builtin_cmd(cls_cmd, "clear");
	add_builtin_cmd(shell_exec_elf, "exec");
	add_builtin_cmd(ps, "ps");
	add_builtin_cmd(kill_cmd, "kill");
	add_builtin_cmd(pwd_cmd, "pwd");
	add_builtin_cmd(cd_cmd, "cd");

	//disable_cursor(0);
	
	print_logo();
	//print_logo_serial();

	for(;;)
	{
		//time_t curr_time = time(NULL);
		printf("\n");
		pwd_cmd();
		printf(">");
		
		fflush(stdout);
		fflush(stdin);
		
		//scanf("%s", buffer);
		getline(buffer, 256);
		//printf("\n");
		if(exec_builtin_cmd(buffer) != 0)
		{
			if (buffer[0] == '/') {
				if(exec_elf(buffer, 0) != 0)
					printf("commande introuvable.\n");
			} else {
				char temp[278];
				sprintf(temp, "/bin/%s", buffer);
				if(exec_elf(temp, 0) != 0)
					printf("commande introuvable.\n");
			}
		}

	}
}
