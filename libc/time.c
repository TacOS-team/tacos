#include <time.h>
#include <stdio.h>

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
	return -1;
}

double difftime(time_t time1, time_t time0)
{
	return -1;
}

time_t mktime(struct tm *timeptr)
{
	return -1;
}

time_t time(time_t *timer)
{
	return -1;
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
