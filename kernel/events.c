/**
 * @file events.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012 - TacOS developers.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * @section DESCRIPTION
 *
 * @brief Gestion d'évènements datés.
 */

#include <i8254.h>
#include <interrupts.h>
#include <list.h>
#include <events.h>
#include <debug.h>

#define MAX_EVENTS 256 /**< Nombre d'évènements au maximum. */

//static const int TIMER_FREQ = I8254_MAX_FREQ / 1000;
//static int ticks;
static list_t events;
static struct event_t scheduler_event;

int compare_events(void* a, void* b)
{
	struct event_t *event_a = (struct event_t *) a;
	struct event_t *event_b = (struct event_t *) b;

	return compare_times(event_a->date, event_b->date);
}

static void events_interrupt(int interrupt_id __attribute__ ((unused)))
{
	struct event_t *event;
	callback_t callback;

	clock_tick();

	event = (struct event_t *) list_get_top(events);
	while(event != NULL && compare_times(event->date, get_tv()) <= 0)
	{
		list_remove_top(&events);
		event->callback(event->data);
		event = (struct event_t *) list_get_top(events);
	}

	if(scheduler_event.callback != NULL 
		&& compare_times(scheduler_event.date, get_tv()) <= 0) {
		callback = scheduler_event.callback;
		unset_scheduler_event();
		callback(scheduler_event.data);
	}
}

void events_init()
{
	unset_scheduler_event();

	clock_init();
	list_init(	&events, 
			(cmp_func_type)compare_events, 
			sizeof(struct event_t), 
			MAX_EVENTS);

	interrupt_set_routine(IRQ_TIMER, events_interrupt, 0);
	//ticks = TIMER_FREQ;
	i8254_init(1000/*TIMER_FREQ*/);
}

void set_scheduler_event(callback_t call, void *data, time_t dtime_usec) {
	scheduler_event.date = get_tv();
	timeval_add_usec(&(scheduler_event.date), dtime_usec);

	scheduler_event.callback = call;
	scheduler_event.data = data;
}

void unset_scheduler_event() {
	scheduler_event.callback = NULL;
}

int add_event(callback_t call, void* data, time_t dtime_usec)
{
	static int id = 0;
	struct event_t event;
	
	event.date = get_tv();

	timeval_add_usec(&(event.date), dtime_usec);

	event.callback = call;
	event.data = data;											
	event.id = id;
	list_add_element(&events, &event);

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
	return list_del_element(&events, id, identifier);
}

