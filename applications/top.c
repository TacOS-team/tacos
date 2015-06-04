/**
 * @file top.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2015 TacOS developers.
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
 * Statistiques utilisation du système par les programmes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

static int first;
unsigned long long prev_tot = 0;
unsigned long long tot = 0;
unsigned long long prev_cpu_time = 0;
unsigned long long cpu_time = 0;

char *padding = "     ";

struct process {
	int pid;
	char name[255];
	unsigned long long utime;
	unsigned long long stime;
	int percent;
	char state[2];
};

unsigned long long values[32768];
struct process tab_process[32768];
unsigned int nb_process;
unsigned int nb_running;
unsigned int nb_sleeping;
unsigned int nb_terminated;

void compute(int pid) {
	struct process* p = &tab_process[nb_process++];

	p->utime = p->stime = 0; // XXX: temporaire le temps d'avoir un scanf %llu

	char path[100];
	sprintf(path, "/proc/%d/stat", pid);

	FILE *f = fopen(path, "r");
	fscanf(f, "%d %s %s %u %u", &p->pid, p->name, p->state, &p->stime, &p->utime);
	fclose(f);

	switch (p->state[0]) {
		case 'R': nb_running++; break;
		case 'W': nb_sleeping++; break;
		case 'T': nb_terminated++; break;
	}

	if (!first) {
		p->percent = (100 * (double)(p->utime + p->stime - values[pid]) / (tot - prev_tot)) + .5;
	} else {
		p->percent = -1;
	}

	values[pid] = p->utime + p->stime;
}

int main() {
	DIR* dir = opendir("/proc");
	first = 1;
	while (1) {
		nb_process = nb_running = nb_terminated = nb_sleeping = 0;

		char name[100];
		unsigned long long user, nice, system, idle;
		FILE* f = fopen("/proc/stat", "r");
		fscanf(f, "%s %u %u %u %u", name, &user, &nice, &system, &idle);
		tot = user + nice + system + idle;
		cpu_time = user + nice + system;
		fclose(f);
		rewinddir(dir);	

		struct dirent *d;
		while ((d = readdir(dir)) != NULL) {
			int pid = atoi(d->d_name);
			if (pid >= 0) {
				compute(pid);
			}
		}

		// display:
		
		// clear screen:
		printf("\e[1;1H\e[2J");
		printf("Tasks: %d total, %d running, %d sleeping, %d terminated\n", nb_process, nb_running, nb_sleeping, nb_terminated);
		printf("CPU: %d%%\n",  (int)(100 * (double)(cpu_time - prev_cpu_time) / (tot - prev_tot) + .5));
		printf("\n");
		printf("\033[30m\033[107m   PID  %%CPU  TIME  COMMAND                                                     \n\033[0m");
		unsigned int i;
		for (i = 0; i < nb_process; i++) {
			struct process* p = &tab_process[i];

			if (p->state[0] == 'T') {
				printf("\033[30m");
			}
			char *padding_pid = padding;
			if (p->pid > 9) padding_pid++;
			if (p->pid > 99) padding_pid++;
			if (p->pid > 999) padding_pid++;

			char *padding_percent = padding;
			if (p->percent < 0 || p->percent > 9) padding_percent++;
			if (p->percent > 99) padding_percent++;

			unsigned long long time = p->utime + p->stime;
			char *padding_time = padding;
			if (time > 9) padding_time++;
			if (time > 99) padding_time++;
			if (time > 999) padding_time++;
			if (time > 9999) padding_time++;
			if (time > 99999) padding_time++;

			printf("%s%d%s%d%s%llu  %s\n", padding_pid, p->pid, padding_percent, p->percent, padding_time, time, p->name);
			if (p->state[0] == 'T') {
				printf("\033[0m");
			}
		}
		
		prev_tot = tot;
		prev_cpu_time = cpu_time;

		first = 0;
		sleep(1);
	}

	return 0;
}
