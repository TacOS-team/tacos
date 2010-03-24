#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <types.h>
#include <time.h>

int compare_times(struct timeval a, struct timeval b);
void clock_init();

/*
 *		Ajoute une milliseconde au temps systeme, ne met rien à jour au dela du jour actuel
 */
void clock_tick();
clock_t get_clock();
time_t get_date();
struct timeval get_tv();

#endif

