/**
 * @file time.h
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012 - TacOS developers.
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

#ifndef _TIME_H_
#define _TIME_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

#include <sys/types.h>

struct timeval{
	long int tv_sec;
	long int tv_usec;
};

struct timespec{
	long int tv_sec;
	long int tv_nsec;
};

typedef long int clock_t;
typedef long int time_t;

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

clock_t clock(void);

double difftime(time_t time1, time_t time0);

time_t mktime(struct tm *timeptr);

time_t time(time_t *timer);

char *asctime(const struct tm *timeptr);

extern inline char *ctime(const time_t *timer);

struct tm *gmtime(const time_t *timer);

struct tm *localtime(const time_t *timer);

/* NOT IMPLEMENTED
size_t strftime(char * s,
     size_t maxsize,
     const char * format,
     const struct tm * timeptr);
*/

__END_DECLS

#endif //_TIME_H_
