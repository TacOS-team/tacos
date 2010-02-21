#include <types.h>
#include <stdio.h>
#include <ioports.h>
#include <keyboard.h>
#include <i8254.h>
#include <interrupts.h>
#include <clock.h>

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

static int increaseSec = 0;
static date_t date = {0, 0, 0, 0, 0, 0};

static void clock_tick(int interrupt_id)
{
  increaseSec--;

  if(increaseSec <= 0)
  {
    date.sec++;
    printf("TICK : %dh%dm%ds\n", date.hour, date.minute, date.sec);

    increaseSec = I8254_MAX_FREQ;
    printf("NYYYUU %d\n", increaseSec);
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
  
  i8254_init(I8254_MAX_FREQ);
}

// http://www-ivs.cs.uni-magdeburg.de/~zbrog/asm/cmos.html
void clock_init()
{
  outb(RTC_DATE_OF_MONTH, RTC_REQUEST);
  date.day = inb(RTC_ANSWER);
  outb(RTC_MONTH, RTC_REQUEST);
  date.month = inb(RTC_ANSWER);
  outb(RTC_YEAR, RTC_REQUEST);
  date.year = inb(RTC_ANSWER);

  outb(RTC_HOUR, RTC_REQUEST);
  date.hour = inb(RTC_ANSWER) & 0xFF;
  outb(RTC_MINUTE, RTC_REQUEST);
  date.minute = inb(RTC_ANSWER) & 0x3F;
  outb(RTC_SECOND, RTC_REQUEST);
  date.sec = inb(RTC_ANSWER) & 0x3F;

  printf("TICK : %dh%dm%ds\n", date.hour, date.minute, date.sec);

  increaseSec = I8254_MAX_FREQ;
  i8254_init(I8254_MAX_FREQ);
  interrupt_set_routine(IRQ_TIMER, clock_tick);
}

