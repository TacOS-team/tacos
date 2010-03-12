#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <types.h>

typedef struct
{
  uint8_t day;
  uint8_t month;
  uint16_t year;
  
  uint8_t hour;
  uint8_t minute;
  uint8_t sec;
} date_t;

int compare_times(date_t a, date_t b);
void clock_init();
void clock_tick();
date_t get_date();

#endif

