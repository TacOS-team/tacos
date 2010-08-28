#include <types.h>
#include <i8254.h>
#include <interrupts.h>
#include <time.h>
#include <clock.h>
#include <list.h>
#include <events.h>
#include <stdio.h>
#include <debug.h>

#define MAX_EVENTS 256

//static const int TIMER_FREQ = I8254_MAX_FREQ / 1000;
//static int ticks;
static list_t events;

int compare_events(void* a, void* b)
{
  struct event_t *event_a = (struct event_t *) a;
  struct event_t *event_b = (struct event_t *) b;

  return compare_times(event_a->date, event_b->date);
}

static void events_interrupt(int interrupt_id __attribute__ ((unused)))
{
	struct event_t *event;

	clock_tick();

	event = (struct event_t *) listGetTop(events);
	while(event != NULL && compare_times(event->date, get_tv()) <= 0)
	{
		listRemoveTop(&events);
		event->callback(event->data);
		event = (struct event_t *) listGetTop(events);
	}
}

void events_init()
{
  clock_init();
  initList(	&events, 
			(cmp_func_type)compare_events, 
			sizeof(struct event_t), 
			MAX_EVENTS);

  interrupt_set_routine(IRQ_TIMER, events_interrupt, 0);
  //ticks = TIMER_FREQ;
  i8254_init(1000/*TIMER_FREQ*/);
}

int add_event(callback_t call, void* data, time_t dtime_usec)
{
	static int id = 0;
	struct event_t event;
	int overflow=0;
	struct timeval clock = get_tv();
	
	event.date.tv_sec = clock.tv_sec;
	event.date.tv_usec = clock.tv_usec + dtime_usec;

	if(event.date.tv_usec > USEC_PER_SEC)
	{
		overflow = event.date.tv_usec/USEC_PER_SEC;
		event.date.tv_usec = event.date.tv_usec%USEC_PER_SEC;
		event.date.tv_sec += overflow;
	}

	event.callback = call;
	event.data = data;											
	event.id = id;
//		kprintf("TIME : %d %d\n", clock.tv_sec, clock.tv_usec);
//		kprintf("NYU ## %d %d %d\n", events.nb_elements, event.date.tv_sec, event.date.tv_usec);
	listAddElement(&events, &event);
//		kprintf("BZZ ## %d %d %d\n", events.nb_elements, ((struct event_t *) (events.elements_array))[events.head].date.tv_sec, ((struct event_t *) (events.elements_array))[events.head].date.tv_usec);

	id++;

	return id-1;
}

static int identifier(int id, void* element)
{
  struct event_t *event_element = (struct event_t *) element;
  return event_element->id == id;
}

int del_event(int id)
{
	return listDelElement(&events, id, identifier);
}

