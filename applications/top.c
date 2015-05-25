/**
 * @file top.c
 *
 * @author TacOS developers 
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
 * Statistiques utilisation du système par les programmes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

unsigned long long values[32768];
unsigned long long prev_tot = 0;
unsigned long long tot = 0;

void compute(int p) {
	int pid;//, ppid, pgrp, session, tty, tpgid;
	char name[100];
	//char state;
	//unsigned long flags, min_flt, cmin_flt, maj_flt, cmaj_flt;
	unsigned long long utime = 0, stime = 0;


	char path[100];
	sprintf(path, "/proc/%d/stat", p);

	FILE *f = fopen(path, "r");
	/*
	fscanf(f, "%d %s %c %d %d %d %d %d"
		  "%lu %lu %lu %lu %lu"
		  "%llu %llu",
		&pid, name, &state, &ppid, &pgrp, &session, &tty, &tpgid,
		&flags, &min_flt, &cmin_flt, &maj_flt, &cmaj_flt,
		&utime, &stime);
		*/
	//fscanf(f, "%d %s %llu %llu", &pid, name, &stime, &utime);
	fscanf(f, "%d %s %u %u", &pid, name, &stime, &utime);

	fclose(f);

	if (values[pid]) {
		int percent = (100 * (double)(utime + stime - values[pid]) / (tot - prev_tot)) + .5;
		printf("%d %s %d %llu\n", pid, name, percent, utime + stime);
	}

	values[pid] = utime + stime;
}

int main() {
	while (1) {
		printf("\e[1;1H\e[2J");
		char name[100];
		unsigned long long user, nice, system, idle;
		FILE* f = fopen("/proc/stat", "r");
		//fscanf(f, "%s %llu %llu %llu %llu", name, &user, &nice, &system, &idle);
		fscanf(f, "%s %u %u %u %u", name, &user, &nice, &system, &idle);
		tot = user + nice + system + idle;
		fclose(f);
		//rewinddir(dir);	
		DIR* dir = opendir("/proc");

		printf("PID COMMAND CPU TIME\n");

		struct dirent *d;
		while ((d = readdir(dir)) != NULL) {
			int pid = atoi(d->d_name);
			if (pid >= 0) {
				compute(pid);
			}
		}
		
		prev_tot = tot;

		sleep(1);
	}

	return 0;
}
