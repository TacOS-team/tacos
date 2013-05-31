/**
 * @file clock.c
 *
 * @author TacOS developers 
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
 * @brief Gestion de l'horloge et du temps en général.
 */

#include <types.h>
#include <ioports.h>
#include <i8254.h>
#include <clock.h>
#include <interrupts.h>
#include <kstdio.h>
#include <klibc/limits.h>
#include "heap.h"

// Note: RTC signifie Real Time Clock.

#define RTC_REQUEST 0x70 /**< RTC select port */
#define RTC_ANSWER  0x71 /**< RTC read/write port */

#define RTC_SECOND        0x00 /**< select seconds */
#define RTC_SECOND_ALARM  0x01 /**< select seconds alarm */
#define RTC_MINUTE        0x02 /**< select minutes */
#define RTC_MINUTE_ALARM  0x03 /**< select minutes alarm */
#define RTC_HOUR          0x04 /**< select hours */
#define RTC_HOUR_ALARM    0x05 /**< select hours alarm */
#define RTC_DAY_OF_WEEK   0x06 /**< select day of week */
#define RTC_DATE_OF_MONTH 0x07 /**< select day of month */
#define RTC_MONTH         0x08 /**< select month */
#define RTC_YEAR          0x09 /**< select year */

#define LEAPYEAR(year) (!((year) % 4) && (((year) % 100) || !((year) % 400))) /**< Macro function pour déterminer si une année est bissextile. */
#define YEARSIZE(year) (LEAPYEAR(year) ? 366 : 365) /**< Nombre de jour dans une année. */
#define YEAR0 1900 /**< Constante pour convertir les années exprimées depuis 1900 */
#define EPOCH_YR   1970 /**< Constante pour convertir les années exprimées depuis 1970. */
#define SECS_DAY   (24L * 60L * 60L) /**< Nombre de secondes dans un jour. */
#define TIME_MAX   0xFFFFFFFFL /**< Temps maximum. */

static clock_t sysclock; /**< Nombre de ticks d'horloge. */
static time_t systime; /**< Date en secondes. */

time_t boot_systime;

const int _ytab[2][12] = { { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
                           { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }};

void clock_tick()
{
	sysclock++;
	if(sysclock>= CLOCKS_PER_SEC)
	{
		sysclock -= CLOCKS_PER_SEC;
		systime++;
	}
}

// nombres à 2 chiffres donc :
// highbyte * 10 + lowbyte
uint8_t bcd2binary(uint8_t n) 
{
  return 10*((n & 0xF0) >> 4) + (n &0x0F);
}

/*
 * adapté de : http://www.raspberryginger.com/jbailey/minix/html/mktime_8c-source.html
 */
time_t clock_mktime(struct tm *timep)
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

// http://www-ivs.cs.uni-magdeburg.de/~zbrog/asm/cmos.html
void clock_init()
{
  struct tm date;
  systime = 0;

  outb(RTC_DATE_OF_MONTH, RTC_REQUEST);
  date.tm_mday = bcd2binary(inb(RTC_ANSWER));
  outb(RTC_MONTH, RTC_REQUEST);
  date.tm_mon = bcd2binary(inb(RTC_ANSWER))-1;
  outb(RTC_YEAR, RTC_REQUEST);
  date.tm_year = bcd2binary(inb(RTC_ANSWER));
  date.tm_year += 100;
  outb(RTC_HOUR, RTC_REQUEST);
  date.tm_hour = bcd2binary(inb(RTC_ANSWER));
  outb(RTC_MINUTE, RTC_REQUEST);
  date.tm_min = bcd2binary(inb(RTC_ANSWER));
  outb(RTC_SECOND, RTC_REQUEST);
  date.tm_sec = bcd2binary(inb(RTC_ANSWER));
  
  systime = clock_mktime(&date);
  boot_systime = systime;
}

clock_t get_clock()
{
	return sysclock;
}

SYSCALL_HANDLER1(sys_getclock, clock_t* clock)
{
	*clock = get_clock();
}

inline time_t get_date()
{
	return systime;
}

SYSCALL_HANDLER1(sys_getdate, time_t* date)
{
	*date = get_date();
}

struct timeval get_tv()
{
	struct timeval ret;
	ret.tv_sec = get_date();
	ret.tv_usec = get_clock()*USEC_PER_SEC/CLOCKS_PER_SEC;
	return ret;
}

int compare_times(struct timeval a, struct timeval b)
{
	if(a.tv_sec != b.tv_sec)
	{
	    return (a.tv_sec < b.tv_sec) ? -1 : 1;
	}
	if(a.tv_usec != b.tv_usec)
	{
	    return (a.tv_usec < b.tv_usec) ? -1 : 1;
	}
	
	return 0;
}

void timeval_add_usec(struct timeval *t, time_t usec) {
	int overflow=0;
	
	t->tv_usec += usec;

	if(t->tv_usec > USEC_PER_SEC)
	{
		overflow = t->tv_usec/USEC_PER_SEC;
		t->tv_usec = t->tv_usec%USEC_PER_SEC;
		t->tv_sec += overflow;
	}
}

void klog_systime() {
	int decimales = (int)(((float)sysclock/(float)CLOCKS_PER_SEC)*100.0f);
	kprintf("[%d.", get_date() - boot_systime);
	if(decimales <10) {
		kprintf("0%d]", decimales);
	} else {
		kprintf("%d]", decimales);
	}
}
