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

size_t strftime(char * s, size_t maxsize, const char * format, const struct tm * timeptr)
{
	return 0;
}
