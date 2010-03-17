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
  uint16_t msec;
} date_t;

/*
 * 	Ajoute dt millisecondes à d
 * 	Cette fonction ne met pas à jour le mois et ne prend pas de valeur plus longues que 23h59m59s999ms
 */
void add_msec(date_t* d, uint32_t dt);


int compare_times(date_t a, date_t b);
void clock_init();

/*
 *		Ajoute une milliseconde au temps systeme, ne met rien à jour au dela du jour actuel
 */
void clock_tick();
date_t get_date();

#endif

