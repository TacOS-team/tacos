/**
 * @file top.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2014 TacOS developers.
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
#include <time.h>
#include <unistd.h>

void kill_handler()
{
	static int try = 1;
	if(try>0)
	{
		printf("essaye encore...\n");
		try--;
	}
	else
	{
		exit(0);
	}
}
		
	

void ps()
{
	process_t* aux = get_process_list(FIRST_PROCESS);
	
	long int ms;
	int s;
	int m;
	int h;
	printf("pid\tname\t\ttime\t\t%CPU\tstate\n");   
	while(aux!=NULL)
	{
			
			/* Calcul du temps d'execution du processus */
			ms = aux->user_time;
			s = ms / 1000;
			
			m = s / 60;
			s = s % 60;
			h = m / 60;
			m = m % 60;
			
			/*if (aux->process == active_process) {
					printf("*");
			}*/
			
			printf("%d\t%s\t\t%dh %dm %ds\t%d\%\t",aux->pid, aux->name, h, m ,s, (int)(((float)aux->last_sample/(float)CPU_USAGE_SAMPLE_RATE)*100.f));
			
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
}

int main()
{
	signal(SIGINT, kill_handler);
	while(1)
	{
		
		printf("\033[1;1H");
		ps();
		sleep(1);
	}
}
