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
  date.tm_mon = bcd2binary(inb(RTC_ANSWER));
  outb(RTC_YEAR, RTC_REQUEST);
  date.tm_year = bcd2binary(inb(RTC_ANSWER));
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

time_t get_date()
{
	return systime;
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
