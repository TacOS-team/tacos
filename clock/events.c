#include <types.h>
#include <i8254.h>
#include <interrupts.h>
#include <clock.h>
#include <heap.h>
#include <events.h>
#include <stdio.h>

#define MAX_EVENTS 256

static const int TIMER_FREQ = I8254_MAX_FREQ / 1000;
static int ticks;
static heap_t events;
static struct event_t events_buffer[MAX_EVENTS];

int compare_events(void* a, void* b)
{
  struct event_t *event_a = (struct event_t *) a;
  struct event_t *event_b = (struct event_t *) b;

  compare_times(event_a->date, event_b->date);
}

static void events_interrupt(int interrupt_id)
{
  struct event_t *event;
  ticks--;

  if(ticks < 0)
  {
    clock_tick();
  
    event = (struct event_t *) getTop(events);
    while(event != NULL && compare_times(event->date, get_date()) < 0)
    {
      removetop(&events);
      event->callback(event->data);
    }
  
    ticks = TIMER_FREQ;
  }
  
  i8254_init(TIMER_FREQ);
}

void events_init()
{
  clock_init();
  initHeap(&events, (cmp_func_type)compare_events, (void*)events_buffer,
           sizeof(struct event_t), MAX_EVENTS);

  interrupt_set_routine(IRQ_TIMER, events_interrupt);
  ticks = TIMER_FREQ;
  i8254_init(TIMER_FREQ);
}

void add_event(void* call, int time)
{
	// XXX : non implementé
}

