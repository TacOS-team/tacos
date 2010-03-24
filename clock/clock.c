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

#define LEAPYEAR(year) (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year) (LEAPYEAR(year) ? 366 : 365)
#define YEAR0 1900
#define EPOCH_YR   1970
#define SECS_DAY   (24L * 60L * 60L)
#define LONG_MAX   2147483647L
#define TIME_MAX   0xFFFFFFFFL

const int _ytab[2][12] = { { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
                           { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }};

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
  /* DEBUG
  outb(RTC_YEAR, RTC_REQUEST);
  date.tm_year = bcd2binary(inb(RTC_ANSWER));
  */
  date.tm_year = 110;
  outb(RTC_HOUR, RTC_REQUEST);
  date.tm_hour = bcd2binary(inb(RTC_ANSWER));
  outb(RTC_MINUTE, RTC_REQUEST);
  date.tm_min = bcd2binary(inb(RTC_ANSWER));
  outb(RTC_SECOND, RTC_REQUEST);
  date.tm_sec = bcd2binary(inb(RTC_ANSWER));
  
  systime = clock_mktime(&date);
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
	unsigned dst;

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
	seconds = 0;
	day = 0;                        /* means days since day 0 now */
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

	if ((TIME_MAX - seconds) / SECS_DAY < day) overflow++;
	seconds += day * SECS_DAY;

	if (overflow)
		return (time_t)-1;

	if ((time_t)seconds != seconds)
		return (time_t)-1;
		
	return (time_t)seconds;
}

struct tm * clock_gmtime(register const time_t *timer)
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
	while (dayno >= _ytab[LEAPYEAR(year)][timep->tm_mon]) {
		dayno -= _ytab[LEAPYEAR(year)][timep->tm_mon];
		timep->tm_mon++;
	}
	timep->tm_mday = dayno + 1;
	timep->tm_isdst = 0;

	return timep;
}
