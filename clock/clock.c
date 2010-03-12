#include <types.h>
#include <stdio.h>
#include <ioports.h>
#include <keyboard.h>
#include <i8254.h>
#include <interrupts.h>
#include <clock.h>
#include <heap.h>

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

static const int TIMER_FREQ = I8254_MAX_FREQ/1000;
static int increaseSec = 0;
static date_t date = {0, 0, 0, 0, 0, 0};

static heap_t events;

static void clock_tick(int interrupt_id)
{
  increaseSec--;

  if(increaseSec <= 0)
  {
    date.sec++;
    increaseSec = TIMER_FREQ;
    
    if(date.sec == 60)
    {
      date.sec = 0;
      date.minute++;
      if(date.minute == 60)
      {
        date.minute = 0;
        date.hour++;
      }
    }
  }
  
  i8254_init(TIMER_FREQ);
}

// nombres à 2 chiffres donc :
// highbyte * 10 + lowbyte
uint8_t bcd2binary(uint8_t n) 
{
  return 10*((n & 0xF0) >> 4) + (n &0x0F);
}

int compare_times(void* a, void* b)
{
	return 1; // not implemented
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

  increaseSec = TIMER_FREQ;
  i8254_init(TIMER_FREQ);
  interrupt_set_routine(IRQ_TIMER, clock_tick);

  initHeap(&events, (cmp_func_type)compare_times);
}

date_t get_date()
{
	return date;
}

void add_event(void* call, int time)
{
	// non implementé
}

