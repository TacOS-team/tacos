#include <types.h>
#include <ioports.h>
#include <i8254.h>
#include <clock.h>
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

#define MAX_EVENTS 256

static date_t date = {0, 0, 0, 0, 0, 0};

static heap_t events;
static date_t events_buffer[MAX_EVENTS];

static uint64_t systime;
static uint32_t increaseSec;

static void clock_tick(int interrupt_id)
{
  systime++;
  increaseSec--;

  if(increaseSec <= 0)
  {
    date.sec++;
    increaseSec = 1000;
    
    if(date.sec == 60)
    {
      date.minute = 0;
      date.hour++;
    }
  }
  
  systime = 0;
  i8254_init(I8254_MAX_FREQ/1000);
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

  increaseSec = 1000;
  i8254_init(I8254_MAX_FREQ/1000);
  interrupt_set_routine(IRQ_TIMER, clock_tick);
  
  initHeap(&events, (cmp_func_type)compare_times, (void*)events_buffer, sizeof(date_t),MAX_EVENTS);
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
	
	return 0;
}
