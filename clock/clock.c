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

#define A_SMALLER  1
#define B_SMALLER -1

#define MILLISECONDS_PER_SECOND 1000
#define SECONDS_PER_MINUTE 60
#define MINUTES_PER_HOUR 60
#define HOURS_PER_DAY 24
#define MONTH_PER_YEAR 12

#define MILLISECONDS_PER_MINUTE MILLISECONDS_PER_SECOND*SECONDS_PER_MINUTE
#define MILLISECONDS_PER_HOUR MILLISECONDS_PER_MINUTE*MINUTES_PER_HOUR
#define MILLISECONDS_PER_DAY MILLISECONDS_PER_HOUR*HOURS_PER_DAY

static date_t date = {0, 0, 0, 0, 0, 0, 0};

static uint64_t systime;

static unsigned int month_length(date_t* d)
{
	unsigned int ret = 31;
	
	if(d->month > 0)
	{
		if(d->month <= 7)
		{
			if(d->month == 2)
				ret = (d->year%4==0 && d->year%100!=0) || (d->year%400==0) ? 29:28;
			else
				ret = d->month%2 ? 30:31;
		}
		else
		{
				ret = d->month%2 ? 31:30;
		}
	}
	
	return ret;
}

static void sanitize_date(date_t* d)
{
	while(d->msec >= MILLISECONDS_PER_SECOND)
	{
		d->msec -= MILLISECONDS_PER_SECOND;
		d->sec++;
	}
	while(d->sec >= SECONDS_PER_MINUTE)
	{
		d->sec -= SECONDS_PER_MINUTE;
		d->minute++;
	}
	while(d->minute >= MINUTES_PER_HOUR)
	{
		d->minute -= MINUTES_PER_HOUR;
		d->hour++;
	}
	while(d->hour >= HOURS_PER_DAY)
	{
		d->hour -= HOURS_PER_DAY;
		d->day++;
	}
	while(d->day > month_length(d))
	{
		d->day -= month_length(d);
		d->month++;
		
		while(d->month >= MONTH_PER_YEAR)
		{
			d->month -= MONTH_PER_YEAR;
			d->year++;		
		}
	}
}

void clock_tick()
{
  systime++;

  date.msec++;
  sanitize_date(&date);
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
  outb(RTC_DATE_OF_MONTH, RTC_REQUEST);
  date.day = bcd2binary(inb(RTC_ANSWER));
  outb(RTC_MONTH, RTC_REQUEST);
  date.month = bcd2binary(inb(RTC_ANSWER));
  outb(RTC_YEAR, RTC_REQUEST);
  date.year = bcd2binary(inb(RTC_ANSWER));

  outb(RTC_HOUR, RTC_REQUEST);
  date.hour = bcd2binary(inb(RTC_ANSWER));
  outb(RTC_MINUTE, RTC_REQUEST);
  date.minute = bcd2binary(inb(RTC_ANSWER));
  outb(RTC_SECOND, RTC_REQUEST);
  date.sec = bcd2binary(inb(RTC_ANSWER));
  
  date.msec = 0;
  
  systime = 0;
}

date_t get_date()
{
	return date;
}

int compare_times(date_t a, date_t b)
{
	if(a.year != b.year)
	{
		if(a.year < b.year)
			return A_SMALLER;
		else
			return B_SMALLER;
	}
	if(a.month != b.month)
	{
		if(a.month < b.month)
			return A_SMALLER;
		else
			return B_SMALLER;
	}
	if(a.day != b.day)
	{
		if(a.day < b.day)
			return A_SMALLER;
		else
			return B_SMALLER;
	}
	if(a.hour != b.hour)
	{
		if(a.hour < b.hour)
			return A_SMALLER;
		else
			return B_SMALLER;
	}
	if(a.minute != b.minute)
	{
		if(a.minute < b.minute)
			return A_SMALLER;
		else
			return B_SMALLER;
	}
	if(a.sec != b.sec)
	{
		if(a.sec < b.sec)
			return A_SMALLER;
		else
			return B_SMALLER;
	}
	if(a.msec != b.msec)
	{
		if(a.msec < b.msec)
			return A_SMALLER;
		else
			return B_SMALLER;
	}
	
	return 0;
}

void add_msec(date_t* d, uint32_t dt)
{
	uint32_t tmp_unit;
	
	// On ignore les durées plus longues qu'une journée
	if(dt >= MILLISECONDS_PER_DAY)
	{
		dt = dt - dt/(MILLISECONDS_PER_DAY);
	}
	// On ajoute les heures
	if(dt >= MILLISECONDS_PER_HOUR)
	{
		tmp_unit = dt/(MILLISECONDS_PER_HOUR);
		d->hour += tmp_unit;
		dt = dt - MILLISECONDS_PER_HOUR*tmp_unit;
	}
	// On ajoute les minutes
	if(dt >= MILLISECONDS_PER_MINUTE)
	{
		tmp_unit = dt/(MILLISECONDS_PER_MINUTE);
		d->minute += tmp_unit;
		dt = dt - MILLISECONDS_PER_MINUTE*tmp_unit;
	}
	// On ajoute les secondes
	if(dt >= MILLISECONDS_PER_SECOND)
	{
		tmp_unit = dt/(MILLISECONDS_PER_SECOND);
		d->sec += tmp_unit;
		dt = dt - MILLISECONDS_PER_SECOND*tmp_unit;
	}
	// On ajoute les millisecondes
	d->msec += dt;
	
	// On corrige la structure
	sanitize_date(d);
}
