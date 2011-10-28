/**
 * @file clock.c
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

#include <types.h>
#include <ioports.h>
#include <i8254.h>
#include <clock.h>
#include <interrupts.h>
#include "heap.h"

#define RTC_REQUEST 0x70
#define RTC_ANSWER  0x71

#define RTC_SECOND        0x00
#define RTC_SECOND_ALARM  0x01
#define RTC_MINUTE        0x02
#define RTC_MINUTE_ALARM  0x03
#define RTC_HOUR          0x04
#define RTC_HOUR_ALARM    0x05
#define RTC_DAY_OF_WEEK   0x06
#define RTC_DATE_OF_MONTH 0x07
#define RTC_MONTH         0x08
#define RTC_YEAR          0x09

#define A_SMALLER -1
#define B_SMALLER 1

static clock_t sysclock;
static time_t systime;

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
  
  systime = mktime(&date);
}

clock_t get_clock()
{
	return sysclock;
}

SYSCALL_HANDLER1(sys_getclock, clock_t* clock)
{
	*clock = get_clock();
}

time_t get_date()
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
		if(a.tv_sec < b.tv_sec)
			return A_SMALLER;
		else
			return B_SMALLER;
	}
	if(a.tv_usec != b.tv_usec)
	{
		if(a.tv_usec < b.tv_usec)
			return A_SMALLER;
		else
			return B_SMALLER;
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
