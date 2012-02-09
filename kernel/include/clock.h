/**
 * @file clock.h
 *
 * @author TacOS developers 
 *
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
 * Gestion de l'horloge et du temps en général.
 */

#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <types.h>
#include <ksyscall.h>

#define CLOCKS_PER_SEC 1000
#define USEC_PER_SEC 1000000

struct timeval{
	long int tv_sec;
	long int tv_usec;
};

struct timespec{
	long int tv_sec;
	long int tv_nsec;
};

struct tm{
	int tm_sec;   //seconds after the minute — [0, 60]
	int tm_min;   //minutes after the hour — [0, 59]
	int tm_hour;  //hours since midnight — [0, 23]
	int tm_mday;  //day of the month — [1, 31]
	int tm_mon;   //months since January — [0, 11]
	int tm_year;  //years since 1900
	int tm_wday;  //days since Sunday — [0, 6]
	int tm_yday;  //days since January 1 — [0, 365]
	int tm_isdst; //Daylight Saving Time flag (1 true, 0 false, neg unavailable)
};

typedef long int time_t;
typedef long int clock_t;

int compare_times(struct timeval a, struct timeval b);
void clock_init();

/*
 *		Ajoute une milliseconde au temps systeme, ne met rien à jour au dela du jour actuel
 */
void clock_tick();
clock_t get_clock();
SYSCALL_HANDLER1(sys_getclock, clock_t* clock);
time_t get_date();
SYSCALL_HANDLER1(sys_getdate, time_t* date);
void timeval_add_usec(struct timeval *t, time_t usec); 
struct timeval get_tv();
time_t clock_mktime(struct tm *timep);
struct tm * clock_gmtime(register const time_t *timer);

/* Exposition en attente de mieux (TODO then)*/
int i8254_init(uint32_t freq);
#endif

