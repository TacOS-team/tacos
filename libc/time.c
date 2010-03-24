#include <time.h>
#include <stdio.h>
#include <clock.h>

#define LEAPYEAR(year) (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year) (LEAPYEAR(year) ? 366 : 365)
#define YEAR0 1900
#define EPOCH_YR   1970
#define SECS_DAY   (24L * 60L * 60L)
#define LONG_MAX   2147483647L
#define TIME_MAX   0xFFFFFFFFL

const int _ytab[2][12] = { { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
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
     static char result[26];
     sprintf(result, "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n",
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

struct tm *gmtime(const time_t *timer)
{
	return NULL;
}

struct tm *localtime(const time_t *timer)
{
	return NULL;
}

size_t strftime(char * s, size_t maxsize, const char * format, const struct tm * timeptr)
{
	return 0;
}
