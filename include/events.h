#ifndef _EVENTS_H_
#define _EVENTS_H_

#include <clock.h>

typedef void *(*callback_t) (void *);

struct event_t
{
  date_t date;
  callback_t callback;
  void *data;
};

void events_init();
void add_event(void* call, int time);

#endif

