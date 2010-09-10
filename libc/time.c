/**
 * @file time.c
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

#include <time.h>
#include <stdio.h>
#include <clock.h>

char *asctime(const struct tm *timeptr)
{
     static const char wday_name[7][3] = {
          "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
     };
     static const char mon_name[12][3] = {
          "Jan", "Feb", "Mar", "Apr", "May", "Jun",
          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
     };
     static char result[26];
     /*
     sprintf(result, "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n",
          wday_name[timeptr->tm_wday],
          mon_name[timeptr->tm_mon],
          timeptr->tm_mday, timeptr->tm_hour,
          timeptr->tm_min, timeptr->tm_sec,
          1900 + timeptr->tm_year);
	*/
		sprintf(result, "%.3s %.3s %d %d:%d:%d %d\n",
          wday_name[timeptr->tm_wday],
          mon_name[timeptr->tm_mon],
          timeptr->tm_mday, timeptr->tm_hour,
          timeptr->tm_min, timeptr->tm_sec,
          1900 + timeptr->tm_year);
          
     return result;
}

inline char *ctime(const time_t *timer)
{
	return asctime(localtime(timer));
}

clock_t clock(void)
{
	return get_clock();
}

time_t time(time_t *timer)
{
	time_t date = get_date();
	
	if(timer != NULL)
		*timer = date;
	return date;
}

double difftime(time_t time1, time_t time0)
{
	return time1-time0;
}

/*
 * adapté de : http://www.raspberryginger.com/jbailey/minix/html/mktime_8c-source.html
 */
time_t mktime(struct tm *timep)
{
	return clock_mktime(timep);
}

struct tm *gmtime(const time_t *timer)
{
	return clock_gmtime(timer);
}

struct tm *localtime(const time_t *timer)
{
	return gmtime(timer);
}
/*
size_t strftime(char * s, size_t maxsize, const char * format, const struct tm * timeptr)
{
	return 0;
}
*/ 
