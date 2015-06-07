/**
 * @file time.c
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
 * Description de ce que fait le fichier
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

#define MILLISECONDS_PER_SECOND 1000 /**< Nombre de millisecondes par secondes. */
#define SECONDS_PER_MINUTE 60 /**< Nombre de secondes par minute. */
#define MINUTES_PER_HOUR 60 /**< Nombre de minutes par heure. */
#define HOURS_PER_DAY 24 /**< Nombre d'heures par jour. */
#define MONTH_PER_YEAR 12 /**< Nombre de mois par année. */

/**
 * Nombre de millisecondes par minute.
 */
#define MILLISECONDS_PER_MINUTE MILLISECONDS_PER_SECOND*SECONDS_PER_MINUTE

/**
 * Nombre de millisecondes par heure.
 */
#define MILLISECONDS_PER_HOUR MILLISECONDS_PER_MINUTE*MINUTES_PER_HOUR

/**
 * Nombre de millisecondes par jour.
 */
#define MILLISECONDS_PER_DAY MILLISECONDS_PER_HOUR*HOURS_PER_DAY

/**
 * Macro qui indique si une année est bissextile.
 */
#define LEAPYEAR(year) (!((year) % 4) && (((year) % 100) || !((year) % 400)))

/**
 * Nombre de jours dans l'année spécifiée.
 */
#define YEARSIZE(year) (LEAPYEAR(year) ? 366 : 365)
#define YEAR0 1900
#define EPOCH_YR   1970
#define SECS_DAY   (24L * 60L * 60L) /** Secondes dans une journée. */
#define LONG_MAX   2147483647L
#define TIME_MAX   0xFFFFFFFFL
#define CLOCKS_PER_SEC 1000 /**< Nombre de secondes par tick d'horloge. */
#define USEC_PER_SEC 1000000 /**< Nombre de us par secondes. */

/**
 * Nombre de jours par mois.
 */
static const int _ytab[2][12] = { { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
                           { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }};




char *asctime(const struct tm *timeptr)
{
     static const char wday_name[7][3] = {
          "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
     };
     static const char mon_name[12][3] = {
          "Jan", "Feb", "Mar", "Apr", "May", "Jun",
          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
     };
     char *result = malloc(26);
     /*
     sprintf(result, "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n",
          wday_name[timeptr->tm_wday],
          mon_name[timeptr->tm_mon],
          timeptr->tm_mday, timeptr->tm_hour,
          timeptr->tm_min, timeptr->tm_sec,
          1900 + timeptr->tm_year);
	*/
		if (timeptr) {
			sprintf(result, "%.3s %.3s %d %d:%d:%d %d\n",
          wday_name[timeptr->tm_wday],
          mon_name[timeptr->tm_mon],
          timeptr->tm_mday, timeptr->tm_hour,
          timeptr->tm_min, timeptr->tm_sec,
          1900 + timeptr->tm_year);
		} else {
			sprintf(result, "Erreur date.");
		}
          
     return result;
}

char *ctime(const time_t *timer)
{
	return asctime(localtime(timer));
}

clock_t clock(void)
{
	clock_t sysclock;

	syscall(SYS_GETCLOCK, (uint32_t)&sysclock, 0, 0);

	return sysclock;
}

time_t time(time_t *timer)
{
	time_t date;

	syscall(SYS_GETDATE, (uint32_t)&date, 0, 0);
	
	if (timer != NULL) {
		*timer = date;
	}

	return date;
}

double difftime(time_t time1, time_t time0)
{
	return time1-time0;
}

int gettimeofday(struct timeval *tv, void *tz_unused __attribute__((unused)))
{
	tv->tv_sec = time(NULL);
	tv->tv_usec = clock()*USEC_PER_SEC/CLOCKS_PER_SEC;
	return 0;
}

/*
 * adapté de : http://www.raspberryginger.com/jbailey/minix/html/mktime_8c-source.html
 */
time_t mktime(struct tm *timep)
{
	long day, year;
	int tm_year;
	int yday, month;
	unsigned long seconds;
	int overflow;

	timep->tm_min += timep->tm_sec / 60;
	timep->tm_sec %= 60;
	if (timep->tm_sec < 0) {
		timep->tm_sec += 60;
		timep->tm_min--;
	}
	timep->tm_hour += timep->tm_min / 60;
	timep->tm_min = timep->tm_min % 60;
	if (timep->tm_min < 0) {
		timep->tm_min += 60;
		timep->tm_hour--;
	}
	day = timep->tm_hour / 24;
	timep->tm_hour= timep->tm_hour % 24;
	if (timep->tm_hour < 0) {
		timep->tm_hour += 24;
		day--;
	}
	timep->tm_year += timep->tm_mon / 12;
	timep->tm_mon %= 12;
	if (timep->tm_mon < 0) {
		timep->tm_mon += 12;
		timep->tm_year--;
	}
	day += (timep->tm_mday - 1);
	while (day < 0) {
		if(--timep->tm_mon < 0) {
				 timep->tm_year--;
				 timep->tm_mon = 11;
		}
		day += _ytab[LEAPYEAR(YEAR0 + timep->tm_year)][timep->tm_mon];
	}
	while (day >= _ytab[LEAPYEAR(YEAR0 + timep->tm_year)][timep->tm_mon]) {
		day -= _ytab[LEAPYEAR(YEAR0 + timep->tm_year)][timep->tm_mon];
		if (++(timep->tm_mon) == 12) {
			timep->tm_mon = 0;
			timep->tm_year++;
		}
	}
	timep->tm_mday = day + 1;
	year = EPOCH_YR;
	if (timep->tm_year < year - YEAR0) return (time_t)-1;
	overflow = 0;
	/* Assume that when day becomes negative, there will certainly
	 * be overflow on seconds.
	 * The check for overflow needs not to be done for leapyears
	 * divisible by 400.
	 * The code only works when year (1970) is not a leapyear.
	 */

	tm_year = timep->tm_year + YEAR0;

	if (LONG_MAX / 365 < tm_year - year) overflow++;
	day = (tm_year - year) * 365;
	if (LONG_MAX - day < (tm_year - year) / 4 + 1) overflow++;
	day += (tm_year - year) / 4
			+ ((tm_year % 4) && tm_year % 4 < year % 4);
	day -= (tm_year - year) / 100
			+ ((tm_year % 100) && tm_year % 100 < year % 100);
	day += (tm_year - year) / 400
			+ ((tm_year % 400) && tm_year % 400 < year % 400);

	yday = month = 0;
	while (month < timep->tm_mon) {
		yday += _ytab[LEAPYEAR(tm_year)][month];
		month++;
	}
	yday += (timep->tm_mday - 1);
	if (day + yday < 0) overflow++;
	day += yday;

	timep->tm_yday = yday;
	timep->tm_wday = (day + 4) % 7;         /* day 0 was thursday (4) */

	seconds = ((timep->tm_hour * 60L) + timep->tm_min) * 60L + timep->tm_sec;

	if ((TIME_MAX - seconds) / SECS_DAY < (unsigned long int)day) overflow++;
	seconds += day * SECS_DAY;

	if (overflow)
		return (time_t)-1;

	/*if ((time_t)seconds != seconds)
		return (time_t)-1;
	*/
	/* affichage de debug
	printf("mktime for\n\
				year : %d\n\
				mon : %d\n\
				day : %d\n\
				hour : %d\n\
				min : %d\n\
				sec : %d\n\
				unix => %d\n\
				",
				timep->tm_year,
				timep->tm_mon,
				timep->tm_mday,
				timep->tm_hour,
				timep->tm_min,
				timep->tm_sec,
				seconds
			);
	*/
	
	return (time_t)seconds;
}

struct tm *gmtime(const time_t *timer)
{
	static struct tm br_time;
	register struct tm *timep = &br_time;
	time_t time = *timer;
	register unsigned long dayclock, dayno;
	int year = EPOCH_YR;

	dayclock = (unsigned long)time % SECS_DAY;
	dayno = (unsigned long)time / SECS_DAY;

	timep->tm_sec = dayclock % 60;
	timep->tm_min = (dayclock % 3600) / 60;
	timep->tm_hour = dayclock / 3600;
	timep->tm_wday = (dayno + 4) % 7;       /* day 0 was a thursday */
	while (dayno >= YEARSIZE(year)) {
		dayno -= YEARSIZE(year);
		year++;
	}
	timep->tm_year = year - YEAR0;
	timep->tm_yday = dayno;
	timep->tm_mon = 0;
	while (dayno >= (unsigned long) _ytab[LEAPYEAR(year)][timep->tm_mon]) {
		dayno -= _ytab[LEAPYEAR(year)][timep->tm_mon];
		timep->tm_mon++;
	}
	timep->tm_mday = dayno + 1;
	timep->tm_isdst = 0;

	/* affichage de debug
	printf("gm for\n\
				unix => %d\n\
				year : %d\n\
				mon : %d\n\
				day : %d\n\
				hour : %d\n\
				min : %d\n\
				sec : %d\n\
				",
				*timer,
				timep->tm_year,
				timep->tm_mon,
				timep->tm_mday,
				timep->tm_hour,
				timep->tm_min,
				timep->tm_sec
			);
	*/
	
	return timep;
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
