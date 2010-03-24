#ifndef _EVENTS_H_
#define _EVENTS_H_

#include <time.h>

typedef void *(*callback_t) (void *);

struct event_t
{
  struct timeval date;
  callback_t callback;
  void *data;
};

void events_init();
void add_event(callback_t call, void* data, clock_t time);

#endif

