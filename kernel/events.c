/**
 * @file events.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2015 TacOS developers.
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
#include <heap.h>
#include <events.h>
#include <debug.h>
#include <scheduler.h>

#define MAX_EVENTS 256 /**< Nombre d'évènements au maximum. */

//static const int TIMER_FREQ = I8254_MAX_FREQ / 1000;
//static int ticks;
static heap_t events;

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
	update_stats();

	event = (struct event_t *) heap_top(&events);
	while(event != NULL && compare_times(event->date, get_tv()) <= 0)
	{
		event->callback(event->data);
		heap_remove(&events);
		event = (struct event_t *) heap_top(&events);
	}
}

void events_init()
{
	clock_init();
	heap_init(	&events, 
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
	
	event.date = get_tv();

	timeval_add_usec(&(event.date), dtime_usec);

	event.callback = call;
	event.data = data;											
	event.id = id;
	asm("cli");
	heap_push(&events, &event);
	asm("sti");
	id++;

	return id-1;
}
